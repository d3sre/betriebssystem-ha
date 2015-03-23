cmake_minimum_required(VERSION 3.1)
project(F05_H_SharedMemoryAccess)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(F05_H_SharedMemoryAccess ${SOURCE_FILES})