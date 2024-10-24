cmake_minimum_required(VERSION 3.14)

project(etl
    VERSION     1.0.0
    DESCRIPTION "Embedded Template Library"
)

option(ETL_BUILD_DOCS "Build docs using Doxygen" OFF)

message(STATUS "ETL_VERSION   : ${etl_VERSION}")
message(STATUS "ETL_BUILD_DOCS: ${ETL_BUILD_DOCS}")

file(GLOB_RECURSE ETL_SOURCES src/*.*)
add_library(etl ${ETL_SOURCES})

target_include_directories(etl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(etl PUBLIC
    ETL_VERSION="${ETL_VERSION}"
)

target_compile_options(etl PUBLIC
	$<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)

install(
    TARGETS etl
    EXPORT etlTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
)

install(
    EXPORT etlTargets
    DESTINATION lib/cmake/etl
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/etlConfigVersion.cmake"
    VERSION ${ETL_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES "${PROJECT_BINARY_DIR}/etlConfigVersion.cmake"
    DESTINATION lib/cmake/etl
)
