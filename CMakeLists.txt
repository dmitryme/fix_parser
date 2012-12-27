cmake_minimum_required(VERSION 2.6)

include(ExternalProject)

project(fix_parser)

if (WIN32)
   include_directories(${CMAKE_SOURCE_DIR}/deps/libxml2/include ${CMAKE_SOURCE_DIR}/deps/iconv/include)
   link_directories(${CMAKE_SOURCE_DIR}/deps/libxml2/lib ${CMAKE_SOURCE_DIR}/deps/iconv/lib)
else(WIN32)
   find_package(LibXml2 REQUIRED)
   include_directories(${LIBXML2_INCLUDE_DIR})
   link_directories(${LIBXML2_LIBRARIES})
endif(WIN32)

include_directories(./include ./src)

if (WIN32)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall")
else(WIN32)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -O2 -Wall")
endif(WIN32)

aux_source_directory(src LIB_SOURCES)
if (WIN32)
   aux_source_directory(src/win LIB_SOURCES)
else(WIN32)
   aux_source_directory(src/lin LIB_SOURCES)
endif(WIN32)

if (WIN32)
   set_source_files_properties(${LIB_SOURCES} PROPERTIES LANGUAGE CXX)
endif(WIN32)

add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
add_library(${PROJECT_NAME}_s STATIC ${LIB_SOURCES})

if (WIN32)
   target_link_libraries(${PROJECT_NAME} libxml2)
   set_target_properties(${PROJECT_NAME}_s PROPERTIES OUTPUT_NAME ${PROJECT_NAME}_s)
else(WIN32)
   target_link_libraries(${PROJECT_NAME} xml2)
   set_target_properties(${PROJECT_NAME}_s PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
endif(WIN32)

if ("${WITH_TESTS}" STREQUAL "YES")
   add_subdirectory(perf_test)
   add_subdirectory(test)

   file(COPY ${fix_parser_SOURCE_DIR}/fix_descr/fix.4.4.xml DESTINATION ${CMAKE_BINARY_DIR}/perf_test)
   file(COPY ${fix_parser_SOURCE_DIR}/fix_descr/fix.4.4.perf.xml DESTINATION ${CMAKE_BINARY_DIR}/perf_test)
   file(COPY ${fix_parser_SOURCE_DIR}/fix_descr DESTINATION ${CMAKE_BINARY_DIR}/test)
   file(COPY ${fix_parser_SOURCE_DIR}/test/data DESTINATION ${CMAKE_BINARY_DIR}/test)
endif("${WITH_TESTS}" STREQUAL "YES")
