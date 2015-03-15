cmake_minimum_required(VERSION 3.1)
project(F03_U_NonBlocking)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    itskylib.h
    main.c)

add_executable(F03_U_NonBlocking ${SOURCE_FILES})