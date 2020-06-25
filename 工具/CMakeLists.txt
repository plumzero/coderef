cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -W")

add_executable(benchmark benchmark.c)
add_executable(euclid euclid.c)
add_executable(gcd gcd.c)
add_executable(nonce_ctr nonce_ctr.c)
add_executable(random64 random64.c)
add_executable(hex_dump hex_dump.c)

add_executable(dec2bin_offset dec2bin_offset.c)
add_executable(dec2chars_len dec2chars_len.c)
add_executable(first_zero first_zero.c)
add_executable(cb_test cb_test.c)
