
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W -g -O2 -std=c++11")

find_package(CURL REQUIRED)

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    link_directories(${CURL_LIBRARIES})
    add_executable(hctest hctest.cpp httpclient.cpp)
    target_link_libraries(hctest curl)
else()
    message(FATAL_ERROR "could not find libcurl module")
endif(CURL_FOUND)

