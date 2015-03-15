cmake_minimum_required(VERSION 3.1)
project(F03_H_DaemonWithNamedPipe)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    include/fsort-metrics.h
    include/fsort.h
    include/hsort.h
    include/isort.h
    include/itskylib.h
    include/msort.h
    include/psort.h
    include/sortcommon.h
    include/sortimpl.h
    include/ternary-hsort.h
    include/transmission-protocols.h
    lib/fsort-metrics.c
    lib/fsort.c
    lib/hsort.c
    lib/isort.c
    lib/lib.c
    lib/msort.c
    lib/psort.c
    lib/sort_common.c
    lib/ternary-hsort.c
    lib/transmission-protocols.c
    main.c)

add_executable(F03_H_DaemonWithNamedPipe ${SOURCE_FILES})