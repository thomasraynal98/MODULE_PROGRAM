#include <sw/redis++/redis++.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <sio_client.h>

#include "mng_lib.h"

using namespace sw::redis;

auto redis = Redis("tcp://127.0.0.1:6379");

sio::socket::ptr current_socket;
sio::client h;

void bind_events(sio::socket::ptr current_socket)
{
    // BIND EVENTS IF EMERGENCY.
}

int main()
{
    //! VARIABLE.
    h.connect("http://localhost:5000/");
    bind_events(h.socket());

    //! CHRONO TIMER VARIABLE
    int frequency       = 1;                              // en Hz.
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

        sio::message::ptr msg_interface = sio::object_message::create();

        msg_interface->get_map()["State_order"]              = sio::string_message::create(*(redis.get("State_order")));
        msg_interface->get_map()["State_module"]             = sio::string_message::create(*(redis.get("State_module")));
        msg_interface->get_map()["State_door"]               = sio::string_message::create(*(redis.get("State_door")));
        msg_interface->get_map()["State_robot"]              = sio::string_message::create(*(redis.get("State_robot")));
        msg_interface->get_map()["State_connection_base"]    = sio::string_message::create(*(redis.get("State_connection_base")));
        msg_interface->get_map()["State_base_identifiant"]   = sio::string_message::create(*(redis.get("State_base_identifiant")));
        msg_interface->get_map()["State_module_identifiant"] = sio::string_message::create(*(redis.get("State_module_identifiant")));
        msg_interface->get_map()["State_error"]              = sio::string_message::create(*(redis.get("State_error")));

        h.socket()->emit("raspberry", msg_interface);
    }
}
