
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -g -O2")

include_directories(.)
add_executable(log_test log_test.c log.c)