CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

# set the project name.
PROJECT(delivery_module_prog VERSION 1.0)

# include directories.
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(src)
include_directories(
     ${CMAKE_SOURCE_DIR}
)

# set good gcc version.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -lredis++ -lhiredis -o CPUUsage")

##################################################################################################################

# COMMUNICATION TO BASE PROGRAM MANAGER.
ADD_EXECUTABLE(communication_mng
    software/communication_mng.cpp
    src/mng_lib.cpp 
    include/mng_lib.h)

TARGET_LINK_LIBRARIES(communication_mng
    redis++
    pthread
    hiredis
    /usr/local/lib/libserial.so)

# REDIS MANAGER.
ADD_EXECUTABLE(redis_mng
software/redis_mng.cpp
src/mng_lib.cpp 
include/mng_lib.h)

TARGET_LINK_LIBRARIES(redis_mng
redis++
pthread
hiredis)

# HARWARE MANAGER.
ADD_EXECUTABLE(hardware_mng
software/hardware_mng.cpp
src/mng_lib.cpp 
include/mng_lib.h)

TARGET_LINK_LIBRARIES(hardware_mng
redis++
pthread
hiredis)