file(READ version.txt ETL_VERSION)
message("ETL_VERSION : ${ETL_VERSION}")

# sources
file(GLOB_RECURSE ETL_SOURCES src/*.*)
add_library(etl ${ETL_SOURCES})

# include dirs
target_include_directories(${PROJECT_NAME}.elf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# defines
target_compile_definitions(${PROJECT_NAME}.elf PUBLIC -DETL_VERSION="${ETL_VERSION}")
