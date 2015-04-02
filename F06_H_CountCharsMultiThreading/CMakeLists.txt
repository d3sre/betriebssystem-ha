cmake_minimum_required(VERSION 3.1)
project(F06_H_CountCharsMultiThreading)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(SOURCE_FILES main.c)
add_executable(F06_H_CountCharsMultiThreading ${SOURCE_FILES})