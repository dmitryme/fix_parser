cmake_minimum_required(VERSION 2.6)

project(perf_test)

aux_source_directory(. PERF_TEST_SOURCES)

add_executable(${PROJECT_NAME} ${PERF_TEST_SOURCES})

if (WIN32)
   set_source_files_properties(${PERF_TEST_SOURCES} PROPERTIES LANGUAGE CXX)
endif(WIN32)

if (WIN32)
   target_link_libraries(${PROJECT_NAME} fix_parser_s libxml2)
else(WIN32)
   target_link_libraries(${PROJECT_NAME} fix_parser_s xml2 rt)
endif(WIN32)
