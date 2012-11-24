cmake_minimum_required(VERSION 2.6)

project(perf_test)

add_executable(${PROJECT_NAME} perf_test.c)

target_link_libraries(${PROJECT_NAME} fix_parser xml2 rt)
