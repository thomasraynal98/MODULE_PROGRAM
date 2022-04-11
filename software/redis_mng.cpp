#include <sw/redis++/redis++.h>
#include <iostream>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>
#include <thread>
#include <chrono>
#include <sio_client.h>

#include "mng_lib.h"

using namespace sw::redis;

auto redis = Redis("tcp://127.0.0.1:6379");
bool debug_ON = true;

int main()
{
    // INIT REDIS VARIABLE
    redis.set("State_order", "OPEN");
    redis.set("State_module", "UNLOCK");
    redis.set("State_door", "CLOSE");
    redis.set("State_robot", "WAITING");
    redis.set("State_connection_base", "NO_CONNECTION");

    redis.set("State_base_identifiant", "XXXXXXXX");
    redis.set("State_module_identifiant", "XXXXXXXX");

    redis.set("State_error", "NO_ERROR");

    // DEBUG MODE

    //! CHRONO TIMER VARIABLE
    int frequency       = 10;                              // en Hz.
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

        if(debug_ON)
        {
            std::system("clear");
            std::cout << "State_order=" << *redis.get("State_order") << std::endl;
            std::cout << "State_module=" << *redis.get("State_module") << std::endl;
            std::cout << "State_door=" << *redis.get("State_door") << std::endl;
            std::cout << "State_robot=" << *redis.get("State_robot") << std::endl;
            std::cout << "State_connection_base=" << *redis.get("State_connection_base") << std::endl;
            std::cout << "State_base_identifiant=" << *redis.get("State_base_identifiant") << std::endl;
            std::cout << "State_module_identifiant=" << *redis.get("State_module_identifiant") << std::endl;
            std::cout << "State_error=" << *redis.get("State_error") << std::endl;
        }
    }
}
