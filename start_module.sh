#!/bin/bash
echo -e '\033[1;33mStart of script demo for delivery module by HiveROBOTICS. (v0.1) \033[0$'
echo -e 'Start Redis-server.'
/home/mod-delivery-1/Dep/redis-stable/src/redis-server &
sleep 1.0
echo -e 'Start redis_mng C++ program.'
/home/mod-delivery-1/Dev/MODULE_PROGRAM/build/redis_mng &
sleep 0.2
echo -e 'Start local server and interface program.'
python3 /home/mod-delivery-1/Dev/KODA_SCREEN_INTERFACE/server.py &
sleep 5.0
cd /home/mod-delivery-1/Dev/KODA_SCREEN_INTERFACE/client
BROWSER=none npm start & 
sleep 30.0
chromium-browser http://localhost:3000/ --start-fullscreen &
sleep 0.2
# echo -e 'Start communication_mng C++ program for base connection.'
# /home/pi/Dev/MODULE_PROGRAM/build/communication_mng &
sleep 0.2
echo -e 'Start hardware_mng C++ program.'
/home/mod-delivery-1/Dev/MODULE_PROGRAM/build/hardware_mng &
sleep 0.2
echo -e 'Start socket_interface C++ program for interface communication.'
/home/mod-delivery-1/Dev/MODULE_PROGRAM/build/socket_com
                