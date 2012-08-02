cmake_minimum_required(VERSION 2.6)

project(fix_parser)

add_definitions(-std=gnu99 -O2 -Wall)

include_directories(./include)
include_directories(/usr/include/libxml2)

aux_source_directory(src SOURCES)
aux_source_directory(test TEST_SOURCES)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
add_executable(test ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME} xml2)
target_link_libraries(test ${PROJECT_NAME} xml2 rt)
