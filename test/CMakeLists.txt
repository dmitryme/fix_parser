cmake_minimum_required(VERSION 2.6)

project(test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

ExternalProject_Add(
    googletest
    SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
    TIMEOUT 10
    # Disable install step
    INSTALL_COMMAND ""
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

target_link_libraries(${PROJECT_NAME} gtest fix_parser pthread xml2 rt)
