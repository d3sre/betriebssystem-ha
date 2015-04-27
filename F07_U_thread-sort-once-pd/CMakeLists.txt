cmake_minimum_required(VERSION 3.1)
project(F07_U_thread_sort_once_pd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c)
add_executable(F07_U_thread_sort_once_pd ${SOURCE_FILES})