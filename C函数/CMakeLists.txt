
cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -g -O2")

add_executable(fgetc fgetc.c)
add_executable(fgets fgets.c)
add_executable(fscanf fscanf.c)
add_executable(getline getline.c)
add_executable(getopt getopt.c)
add_executable(sscanf sscanf.c)
