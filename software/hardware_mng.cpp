#include <iostream>
#include <sw/redis++/redis++.h>
#include <wiringPi.h>
#include <thread>
#include <chrono>

#include "mng_lib.h"

using namespace sw::redis;

auto redis = Redis("tcp://127.0.0.1:6379");

int main()
{
    wiringPiSetup();
    int gpio_id = 0;
    pinMode(gpio_id, OUTPUT);
    std::string current_status = "CLOSE";

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

        if((*(redis->get("State_order"))).compare("OPEN") == 0)
        {
            if(!current_status.compare("OPEN"))
            {
               digitalWrite(gpio_id, true); 
               current_status = "OPEN";
            }
        }
        else
        {
            if(!current_status.compare("CLOSE"))
            {
               digitalWrite(gpio_id, false); 
               current_status = "CLOSE";
            }
        }
        redis->set("State_status_order", "COMPLETED");
    }

    return 0;
}