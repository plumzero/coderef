
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -g -O2 -std=c++11")

add_executable(tai_test tai_test.cpp tai.cpp)
add_executable(tai_conv tai_conv.cpp tai.cpp)
