cmake_minimum_required(VERSION 3.1)
project(F12_H_client_server_exercise)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES socket-thread-sort-shorts-once.c)
add_executable(F12_H_client_server_exercise ${SOURCE_FILES} tcp-square-client.h tcp-square-client.c socket-thread-sort-shorts-once.h socket-thread-sort-shorts-once.c socket-thread-sort-shorts-once-client.h socket-thread-sort-shorts-once-client.c)