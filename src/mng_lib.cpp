#include <sw/redis++/redis++.h>
#include <iostream>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>
#include <thread>
#include <chrono>
#include <unistd.h>

#include "mng_lib.h"

LibSerial::SerialPort* found_micro_port(int debug_mode, bool wait_option)
{
    std::string message = "0/HELLO_BASE\n";

    for (int i=0; i<4; i++)
    {
        LibSerial::SerialPort* serial_port = new LibSerial::SerialPort;
        std::string name_port = "/dev/ttyS" + std::__cxx11::to_string(i);
        bool is_openable = true;

        if(debug_mode==1) {std::cout << "pointeur:" << serial_port <<"\n";}

        try{ serial_port->Open(name_port);}
        catch (LibSerial::AlreadyOpen ex)
        {
            if(debug_mode==1) {std::cout << "SerialPort already open : " << name_port << std::endl;}
            is_openable = false;
        }
        catch (LibSerial::OpenFailed ex)
        {
            if(debug_mode==1) {std::cout << "Failed to open SerialPort : " << name_port << std::endl;}
            is_openable = false;
        }

        if(is_openable)
        {
            if(wait_option){ usleep(500000);}

            if(debug_mode==1) {std::cout << "Succes to open SerialPort : " << name_port << std::endl;}
            if(debug_mode==1) {std::cout << "message:" << message << "\n";}
            
            try{ serial_port->Write(message);}
            catch(std::runtime_error ex) { std::cout << "nop\n"; }
            
            std::string reponse;
            serial_port->ReadLine(reponse);
            if(debug_mode==1) {std::cout << "reponse:" << reponse;}

            if(message.compare(reponse) == 0)
            {   
                // TODOEND.
                std::cout << "[MATCH]" << serial_port << std::endl;
                return serial_port;
            }
            else{serial_port->Close(); std::cout << "[CLOSE_PORT]" << std::endl;}
        }     
    }

    return NULL;
}

void send_ping_micro(LibSerial::SerialPort* connection, std::chrono::high_resolution_clock::time_point* ping_time)
{
    std::string ping_message = "0/MODULE_PING/";
    connection->Write(ping_message);
    *ping_time = std::chrono::high_resolution_clock::now();
}

void get_base_information(sw::redis::Redis* redis, std::string msg)
{
    /*
        msg description: 1/A/B/
        A = identifiant base "RANE_MK3_KODA_1"
        B = robot state "WAITING" "IN_DELIVERY" "REACH_DESTINATION"
    */

    std::string T;
    std::stringstream X(msg);
    
    int i = 0;
    while(std::getline(X, T, '/'))
    {
        if(i == 1) { redis->set("State_base_identifiant", T);}
        if(i == 2) { redis->set("State_robot", T);}
        i += 1;
    }
}

void get_base_order(sw::redis::Redis* redis, std::string msg)
{
    /*
        msg description: 2/A/
        A = identifiant base "OPEN" "CLOSE"
    */

    std::string T;
    std::stringstream X(msg);
    
    int i = 0;
    while(std::getline(X, T, '/'))
    {
        if(i == 1) { redis->set("State_order", T);}
        i += 1;
    }
    redis->set("State_status_order", "IN_PROGRESS");
}

void inform_base(sw::redis::Redis* redis, LibSerial::SerialPort* connection)
{
    /*
        State_status_order
    */

    std::string message = "1/";
    message += *(redis->get("State_module_identifiant")) + "/\n";
    connection->Write(message);
}