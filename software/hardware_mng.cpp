#include <iostream>
#include <sw/redis++/redis++.h>
#include <wiringPi.h>
#include <thread>
#include <chrono>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>

#include "mng_lib.h"

using namespace sw::redis;

auto redis = Redis("tcp://127.0.0.1:6379");

int main()
{
    wiringPiSetup();
    int gpio_id = 15;
    pinMode(gpio_id, OUTPUT);
    int gpio_id2 = 2;
    pinMode(gpio_id2, OUTPUT);

    /*
        DESCRIPTION:
        The main loop program will listen the redis RAM memory system
        and send command to hardware.
    */

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

        if((*(redis.get("State_order"))).compare("OPEN") == 0)
        {
            if((*(redis.get("State_door"))).compare("CLOSE") == 0)
            {
               digitalWrite(gpio_id, true); 
               redis.set("State_door", "OPEN");
            }
        }
        if((*(redis.get("State_order"))).compare("CLOSE") == 0)
        {
            if((*(redis.get("State_door"))).compare("OPEN") == 0)
            {
               digitalWrite(gpio_id, false); 
               redis.set("State_door", "CLOSE");
            }
        }
        if((*(redis.get("State_order"))).compare("LOCK") == 0)
        {
            if((*(redis.get("State_module"))).compare("UNLOCK") == 0)
            {
               digitalWrite(gpio_id2, true); 
               redis.set("State_module", "LOCK");
            }
        }
        if((*(redis.get("State_order"))).compare("UNLOCK") == 0)
        {
            if((*(redis.get("State_module"))).compare("LOCK") == 0)
            {
               digitalWrite(gpio_id2, false); 
               redis.set("State_module", "UNLOCK");
            }
        }

        // TODO: if base ask for UNLOCK OR LOCK ORDER.
    }

    return 0;
}
