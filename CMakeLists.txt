cmake_minimum_required(VERSION 3.16)
project(test)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

file(GLOB TEST_SRCS etl/*.* test/*.*)
add_executable(${PROJECT_NAME} ${TEST_SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE
        include/
        /usr/local/include/gtest/)

target_link_libraries(${PROJECT_NAME} PRIVATE
        m
        Threads::Threads
        /usr/local/lib/libgtest.a)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        ETL_LINKED_LIST_USE_MUTEX=0)

add_subdirectory(docs)
