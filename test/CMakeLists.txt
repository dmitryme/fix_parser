cmake_minimum_required(VERSION 2.6)

project(test)

include_directories(../include)
include_directories(/usr/include/libxml2)

aux_source_directory(. TEST_SOURCES)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME} fix_parser xml2 rt check)