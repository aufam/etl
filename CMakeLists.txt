cmake_minimum_required(VERSION 3.14)

# project config
file(READ version.txt ETL_VERSION)
project(etl VERSION ${ETL_VERSION})

# some messages
message("ETL_VERSION : ${ETL_VERSION}")

# sources
file(GLOB_RECURSE ETL_SOURCES src/*.*)
add_library(etl ${ETL_SOURCES})

# include dirs
target_include_directories(etl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# defines
target_compile_definitions(etl PUBLIC -DETL_VERSION="${ETL_VERSION}")
