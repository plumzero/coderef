cmake_minimum_required(VERSION 2.8)

project("注意" C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -W")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")

aux_source_directory(. SRC_LIST)

set_source_files_properties(${SRC_LIST} PROPERTIES LANGUAGE CXX)

foreach(src ${SRC_LIST})
    string(REGEX REPLACE "^./" "" src ${src})
    
    set(exe ${src})
    string(REGEX REPLACE ".c$" "" exe ${exe})
    
    add_executable(${exe} ${src})
    target_link_libraries(${exe} pthread)
endforeach(src)
