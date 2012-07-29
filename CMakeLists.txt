cmake_minimum_required(VERSION 2.6)

project(fix_parser)

add_definitions(-std=gnu99 -g)

include_directories(./include)
include_directories(/usr/include/libxml2)

aux_source_directory(src SOURCES)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} xml2)
