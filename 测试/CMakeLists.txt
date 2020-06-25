cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-sequence-point -Wno-unused-but-set-variable")

add_executable(big_lit_endian  big_lit_endian.c)
add_executable(byte_align byte_align.c)
add_executable(sizeof_increment sizeof_increment.c)
add_executable(three_dimensional_array three_dimensional_array.c)
