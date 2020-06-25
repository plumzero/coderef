
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -O2 -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-but-set-variable")

find_package(CURL REQUIRED)

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    link_directories(${CURL_LIBRARIES})
    
    add_executable(demo_easy_1 demo_easy_1.c)
    target_link_libraries(demo_easy_1 curl pthread)
    
    add_executable(demo_multi_1 demo_multi_1.c)
    target_link_libraries(demo_multi_1 curl pthread)
    
    add_executable(demo_multi_2 demo_multi_2.c)
    target_link_libraries(demo_multi_2 curl pthread)
    
    add_executable(demo_multi_3 demo_multi_3.c)
    target_link_libraries(demo_multi_3 curl pthread)
	
	find_package(Libevent)
	if (LIBEVENT_FOUND)
		add_executable(demo_multi_libevent demo_multi_libevent.c)
		target_link_libraries(demo_multi_libevent curl pthread)
	endif()
else()
    message(FATAL_ERROR "could not find libcurl module")
endif(CURL_FOUND)

