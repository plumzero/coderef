
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -std=c++11")

add_executable(backslash backslash.cpp)
add_executable(wipeoff_colon wipeoff_colon.cpp)
add_executable(trim_space trim_space.cpp)
add_executable(split split.cpp)
add_executable(strip strip.cpp)
add_executable(strnocasecmp strnocasecmp.cpp)
add_executable(to_array to_array.cpp)
add_executable(rm_header_footer rm_header_footer.cpp)
