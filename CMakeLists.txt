cmake_minimum_required(VERSION 2.6)

project(fix_parser)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   add_definitions(-O2 -Wall)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   add_definitions(-std=gnu99 -O2 -Wall)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

include_directories(./include)
include_directories(/usr/include/libxml2)

aux_source_directory(src LIB_SOURCES)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   set_source_files_properties(&{LIB_SORCES} PROPERTIES LANGUAGE CXX)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_library(${PROJECT_NAME}_S STATIC ${LIB_SOURCES})
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
   set_target_properties(${PROJECT_NAME}_S PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_subdirectory(perf_test)
add_subdirectory(test)

file(COPY ${fix_parser_SOURCE_DIR}/fix_descr/fix.4.4.xml DESTINATION ${CMAKE_BINARY_DIR}/perf_test)
file(COPY ${fix_parser_SOURCE_DIR}/fix_descr/fix.4.4.perf.xml DESTINATION ${CMAKE_BINARY_DIR}/perf_test)
file(COPY ${fix_parser_SOURCE_DIR}/fix_descr DESTINATION ${CMAKE_BINARY_DIR}/test)
file(COPY ${fix_parser_SOURCE_DIR}/test/data DESTINATION ${CMAKE_BINARY_DIR}/test)
