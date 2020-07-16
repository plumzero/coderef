cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-sequence-point -Wno-unused-but-set-variable")

aux_source_directory(. SRC_LIST)

set_source_files_properties(${SRC_LIST} PROPERTIES LANGUAGE CXX)

foreach(src ${SRC_LIST})
    string(REGEX REPLACE "^./" "" src ${src})
    
    set(exe ${src})
    string(REGEX REPLACE ".c$" "" exe ${exe})
    
    add_executable(${exe} ${src})
    target_link_libraries(${exe} pthread)
endforeach(src)

