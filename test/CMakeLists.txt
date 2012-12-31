cmake_minimum_required(VERSION 2.6)

project(test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

ExternalProject_Add(
    googletest
    GIT_REPOSITORY git://github.com/dmitryme/googletest.git
    TIMEOUT 10
    # Disable install step
    INSTALL_COMMAND ""
    CMAKE_ARGS -Dgtest_force_shared_crt=ON
    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(googletest SOURCE_DIR)
ExternalProject_Get_Property(googletest BINARY_DIR)

include_directories(${SOURCE_DIR}/include)
link_directories(${BINARY_DIR})

aux_source_directory(. TEST_SOURCES)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

if (WIN32)
   target_link_libraries(${PROJECT_NAME} gtest fix_parser_s libxml2)
else(WIN32)
   target_link_libraries(${PROJECT_NAME} gtest fix_parser_s pthread xml2 rt)
endif(WIN32)
