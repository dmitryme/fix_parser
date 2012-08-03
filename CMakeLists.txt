cmake_minimum_required(VERSION 2.6)

project(fix_parser)

add_definitions(-std=gnu99 -O2 -Wall)

include_directories(./include)
include_directories(/usr/include/libxml2)

aux_source_directory(src LIB_SOURCES)

add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})

add_subdirectory(perf_test)
add_subdirectory(test)

