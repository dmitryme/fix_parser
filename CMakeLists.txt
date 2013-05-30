cmake_minimum_required(VERSION 2.8)

include(ExternalProject)

project(fix_parser)

if (WIN32)
   set(LIBXML2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/libxml2/include)
   set(LIBXML2_LIBRARIES ${CMAKE_SOURCE_DIR}/deps/libxml2/lib)
else(WIN32)
   find_package(LibXml2 REQUIRED)
endif(WIN32)

include_directories(${LIBXML2_INCLUDE_DIR} ./include ./src)
link_directories(${LIBXML2_LIBRARIES})

if (WIN32)
   add_definitions(-D_CRT_SECURE_NO_WARNINGS)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
else(WIN32)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -fPIC")
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
endif("${WITH_TESTS}" STREQUAL "YES")
