cmake_minimum_required(VERSION 2.8)

project("tmpfile" C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -O2")

add_executable(tmpnam  tmpnam.c)
add_executable(mkstemp mkstemp.c)
