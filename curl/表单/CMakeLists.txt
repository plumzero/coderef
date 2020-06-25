
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -O2 -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")

find_package(CURL REQUIRED)

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    link_directories(${CURL_LIBRARIES})
    
    add_executable(demo_formadd_1 demo_formadd_1.c)
    target_link_libraries(demo_formadd_1 curl)
    
    add_executable(demo_formadd_2 demo_formadd_2.c)
    target_link_libraries(demo_formadd_2 curl)
    
    add_executable(demo_formadd_3 demo_formadd_3.c)
    target_link_libraries(demo_formadd_3 curl)
    
    add_executable(demo_formadd_4 demo_formadd_4.c)
    target_link_libraries(demo_formadd_4 curl)
    
    add_executable(demo_formadd_5 demo_formadd_5.c)
    target_link_libraries(demo_formadd_5 curl)
    
    add_executable(demo_formadd_6 demo_formadd_6.c)
    target_link_libraries(demo_formadd_6 curl)

else()
    message(FATAL_ERROR "could not find libcurl module")
endif(CURL_FOUND)

