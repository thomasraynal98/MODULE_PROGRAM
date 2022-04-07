#!/bin/bash
echo -e '\033[1;33mStart of script demo for delivery module by HiveROBOTICS. (v0.1) \033[0$'
echo -e 'Start Redis-server.'
/home/pi/Dep/redis-stable/src/redis-server &
sleep 0.5
echo -e 'Start redis_mng C++ program.'
# /home/pi/Dev/MODULE_PROGRAM/build/redis_mng &
sleep 0.2
echo -e 'Start communication_mng C++ program for base connection.'
# /home/pi/Dev/MODULE_PROGRAM/build/communication_mng &
echo -e 'Start hardware_mng C++ program.'
# /home/pi/Dev/MODULE_PROGRAM/build/hardware_mng &
echo -e 'Start the screen web interface.'
cd /home/pi/Dev/MODULE_PROGRAM/build/
./interface 
