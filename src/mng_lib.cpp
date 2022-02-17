#include <sw/redis++/redis++.h>
#include <iostream>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>
#include <thread>
#include <chrono>
#include <unistd.h>

#include "mng_lib.h"

void send_ping_micro(LibSerial::SerialPort* connection, std::chrono::high_resolution_clock::time_point* ping_time)
{
    std::string ping_message = "0/MODULE_PING/\n";
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
        msg description: 1/A/
        A = State_module_identifiant "DEL_V1_1"
    */

    std::string message = "1/";
    message += *(redis->get("State_module_identifiant")) + "/\n";
    connection->Write(message);
}

void read_base(sw::redis::Redis* redis, std::string msg)
{
    /*
        msg description: 1/A/B/C/
        A = State_base_identifiant "RANE_MK3_KODA_1"
        B = State_robot "WAITING" "IN_DELIVERY" "REACH_DESTINATION"
        C = State_order "OPEN" "CLOSE" "LOCK" "UNLOCK"
    */

    std::string T;
    std::stringstream X(msg);
    
    int i = 0;
    while(std::getline(X, T, '/'))
    {
        if(i == 1) { redis->set("State_base_identifiant", T);}
        if(i == 2) { redis->set("State_robot", T);}
        if(i == 3) { redis->set("State_order", T);}
        i += 1;
    }
}