cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0 -Wall -W")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable")

add_executable(hex_bin_conv hex_bin_conv.cpp)
add_executable(hex_bin_convert hex_bin_convert.cpp)
add_executable(hex_integral_convert hex_integral_convert.cpp)
add_executable(ipstr_int_convert ipstr_int_convert.cpp)

