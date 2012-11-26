cmake_minimum_required(VERSION 2.6)

include(ExternalProject)

project(fix_parser)

if (WIN32)
   set(LIBXML2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/libxml2/include) 
   include_directories(${CMAKE_SOURCE_DIR}/deps/iconv/include)
   link_directories(${CMAKE_SOURCE_DIR}/deps/libxml2/lib ${CMAKE_SOURCE_DIR}/deps/iconv/lib)
else(WIN32)
   find_package(LibXml2 REQUIRED)
endif(WIN32)

if (WIN32)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")
else(WIN32)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -O2 -Wall")
endif(WIN32)

include_directories(./include ./src ${LIBXML2_INCLUDE_DIR})

aux_source_directory(src LIB_SOURCES)

if (WIN32)
   set_source_files_properties(${LIB_SOURCES} PROPERTIES LANGUAGE CXX)
endif(WIN32)

add_library(${PROJECT_NAME}_S STATIC ${LIB_SOURCES})
add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
set_target_properties(${PROJECT_NAME}_S PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} libxml2)

if ("${WITH_TESTS}" STREQUAL "YES")
   add_subdirectory(perf_test)
   add_subdirectory(test)

   file(COPY ${fix_parser_SOURCE_DIR}/fix_descr/fix.4.4.xml DESTINATION ${CMAKE_BINARY_DIR}/perf_test)
   file(COPY ${fix_parser_SOURCE_DIR}/fix_descr/fix.4.4.perf.xml DESTINATION ${CMAKE_BINARY_DIR}/perf_test)
   file(COPY ${fix_parser_SOURCE_DIR}/fix_descr DESTINATION ${CMAKE_BINARY_DIR}/test)
   file(COPY ${fix_parser_SOURCE_DIR}/test/data DESTINATION ${CMAKE_BINARY_DIR}/test)
endif("${WITH_TESTS}" STREQUAL "YES")