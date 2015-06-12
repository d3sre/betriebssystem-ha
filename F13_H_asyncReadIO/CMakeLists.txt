cmake_minimum_required(VERSION 3.1)
project(F13_H_asyncReadIO)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c)
add_executable(F13_H_asyncReadIO ${SOURCE_FILES})