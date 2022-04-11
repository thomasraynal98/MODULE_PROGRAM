CMAKE_MINIMUM_REQUIRED(VERSION 3.7)

# set the project name.
PROJECT(delivery_module_prog VERSION 1.0)

# include directories.
INCLUDE_DIRECTORIES(include)
#INCLUDE_DIRECTORIES(src)
#include_directories(
#     ${CMAKE_SOURCE_DIR}
#)

# FIND_PACKAGE(OpenCV 4 REQUIRED)
FIND_PACKAGE(websocketpp REQUIRED)

# set good gcc version.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -L/usr/local/lib -lserial -pthread -lwiringPi -o CPUUsage")

##################################################################################################################

# # TEST CODE
# ADD_EXECUTABLE(test_ 
#     software/test.cpp)
# TARGET_LINK_LIBRARIES(test_
#     serial
#     wiringPi)


# COMMUNICATION TO BASE PROGRAM MANAGER.
ADD_EXECUTABLE(communication_mng
    software/communication_mng.cpp
    src/mng_lib.cpp 
    include/mng_lib.h)

TARGET_LINK_LIBRARIES(communication_mng
    redis++
    pthread
    hiredis
    serial)

# REDIS MANAGER.
ADD_EXECUTABLE(redis_mng
software/redis_mng.cpp
src/mng_lib.cpp 
include/mng_lib.h)

TARGET_LINK_LIBRARIES(redis_mng
redis++
pthread
hiredis
serial)


# HARWARE MANAGER.
ADD_EXECUTABLE(hardware_mng
software/hardware_mng.cpp
src/mng_lib.cpp 
include/mng_lib.h)

TARGET_LINK_LIBRARIES(hardware_mng
redis++
pthread
hiredis
wiringPi
serial)

# INTERFACE OPENCV
# ADD_EXECUTABLE(interface
# software/interface.cpp
# src/mng_lib.cpp 
# include/mng_lib.h)

# TARGET_LINK_LIBRARIES(interface
# redis++
# pthread
# hiredis
# serial
# ${OpenCV_LIBS})

# SOCKET INTERFACE
ADD_EXECUTABLE(socket_com
software/socket_interface.cpp
src/mng_lib.cpp 
include/mng_lib.h)

TARGET_LINK_LIBRARIES(socket_com
redis++
pthread
hiredis
serial
sioclient)