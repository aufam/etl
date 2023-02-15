cmake_minimum_required(VERSION 3.16)

project(etl)
include_directories(include/)

# only build docs and test if top level project
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    add_subdirectory(docs)
    add_subdirectory(test)
endif()
