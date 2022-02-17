#include <sw/redis++/redis++.h>
#include <iostream>
#include <libserial/SerialPort.h>
#include <libserial/SerialStream.h>
#include <thread>
#include <chrono>

void send_ping_micro(LibSerial::SerialPort* connection, std::chrono::high_resolution_clock::time_point* ping_time);
void get_base_information(sw::redis::Redis* redis, std::string msg);
void get_base_order(sw::redis::Redis* redis, std::string msg);
void inform_base(sw::redis::Redis* redis, LibSerial::SerialPort* connection);
void read_base(sw::redis::Redis* redis, std::string msg);