cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -W")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")

add_executable(msleep msleep.c)
add_executable(time_conver time_conver.c)
add_executable(time_joint time_joint.c)
add_executable(timer timer.cpp)
