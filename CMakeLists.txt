cmake_minimum_required(VERSION 2.6)

project(fix_parser)

add_definitions(-std=gnu99 -O2 -Wall)

include_directories(./include)
include_directories(/usr/include/libxml2)

aux_source_directory(src LIB_SOURCES)

add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})

add_subdirectory(perf_test)
add_subdirectory(test)

file(COPY ${CMAKE_SOURCE_DIR}/fix_descr/fix.4.4.perf.xml DESTINATION ${CMAKE_BINARY_DIR}/perf_test)
file(COPY ${CMAKE_SOURCE_DIR}/fix_descr DESTINATION ${CMAKE_BINARY_DIR}/test)
file(COPY ${CMAKE_SOURCE_DIR}/test/data DESTINATION ${CMAKE_BINARY_DIR}/test)
