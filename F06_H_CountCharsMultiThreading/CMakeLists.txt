cmake_minimum_required(VERSION 3.1)
project(F06_H_CountCharsMultiThreading)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(F06_H_CountCharsMultiThreading ${SOURCE_FILES})