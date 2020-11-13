
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -g -O2 -std=c++11")

include_directories(.)

add_executable(test_base64 test_base64.cpp)
