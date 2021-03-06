#include <iostream>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <cstring>

#include "mng_lib.h"

using namespace sw::redis;

auto redis = Redis("tcp://127.0.0.1:6379");
std::thread thread_A, thread_B;
std::chrono::high_resolution_clock::time_point ping_time, pong_time;
LibSerial::SerialPort connection;
std::string module_identifiant = "DEL_V1_1";

void talk_function()
{
    //! CHRONO TIMER VARIABLE
    int frequency       = 5;                              // en Hz.
    double time_of_loop = 1000/frequency;                  // en milliseconde.
    std::chrono::high_resolution_clock::time_point last_loop_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point x              = std::chrono::high_resolution_clock::now();
    auto next = std::chrono::high_resolution_clock::now();
    //! CHRONO TIMER VARIABLE

    while(true)
    {
        //! CHRONO TIMER VARIABLE
        x                          = std::chrono::high_resolution_clock::now();         
        last_loop_time             = x;
        next                       += std::chrono::milliseconds((int)time_of_loop);
        std::this_thread::sleep_until(next);
        //! CHRONO TIMER VARIABLE

        try{
            if(connection.IsOpen())
            {
                inform_base(&redis, &connection);
            }
        } catch(...){}
    }
}

void read_function()
{
	// Ensure connection variable.
	auto now  = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> time_span;
    int time_before_disconected = 1000;  

	// Read variable.
    char stop = '\n';
	const unsigned int msTimeout = 100;

    while(true)
    {
        try{
        if(connection.IsOpen())
        {
			std::string reponse;
            connection.ReadLine(reponse, stop, msTimeout);

            std::cout << reponse << std::endl;

			if(reponse.size() > 1)
			{
				if(reponse[0] == '0' || reponse[0] == '1')
				{
					// We get viable message from base.
					read_base(&redis, reponse);
					pong_time = std::chrono::high_resolution_clock::now();
				}
			}

			// We check if we are always connected to base.
			now  = std::chrono::high_resolution_clock::now();
			time_span = now - pong_time;
			if((int)time_span.count() > time_before_disconected)
			{
				// we don't receive ping/pong.
				connection.Close();
				redis.set("State_connection_base", "DISCONNECTED");
				redis.set("State_base_identifiant", "XXXXXXXX");
			}
            else
            {
                redis.set("State_connection_base", "CONNECTED");
            }
        }
        else
        {
            usleep(1000000);
            connection.Open("/dev/ttyS0");
            connection.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
            connection.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
            connection.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
            connection.SetParity(LibSerial::Parity::PARITY_NONE);
        }
        }catch(...){}
    }   
}

int main()
{
    redis.set("State_module_identifiant", module_identifiant);

    connection.Open("/dev/ttyS0");
	redis.set("State_connection_base", "DISCONNECTED");

    thread_A = std::thread(&talk_function);
    thread_B = std::thread(&read_function);

    thread_A.join();
    thread_B.join();
}