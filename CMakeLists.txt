cmake_minimum_required(VERSION 3.16)
project(test_etl)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

file(GLOB TEST_SRCS etl/*.* test/*.*)
add_executable(${PROJECT_NAME} ${TEST_SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_HOME_DIRECTORY}
        /usr/local/include/gtest/)

target_link_libraries(${PROJECT_NAME} PRIVATE
        m
        Threads::Threads
        /usr/local/lib/libgtest.a)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        ETL_LINKED_LIST_USE_MUTEX=0)
