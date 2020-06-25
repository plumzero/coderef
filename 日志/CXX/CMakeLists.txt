cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0 -Wall -W")

include_directories(.)

add_executable(cxxtest cxxtest.cpp cxxlog.cpp)

add_executable(asynctest asynctest.cpp asynclog.cpp)
target_link_libraries(asynctest pthread)

add_executable(synclog synclog.cpp)

