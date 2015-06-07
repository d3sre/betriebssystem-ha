cmake_minimum_required(VERSION 3.1)
project(F13_U_threadsAndPipes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c)
add_executable(F13_U_threadsAndPipes ${SOURCE_FILES} threadsAndPipesNonBlocking.h threadsAndPipesNonBlocking.c multii-threaded-multi-pipe-select.h multii-threaded-multi-pipe-select.c .idea/pipes-async-io.h .idea/pipes-async-io.c)