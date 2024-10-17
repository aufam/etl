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
target_include_directories(etl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# defines
target_compile_definitions(etl PUBLIC
    -DETL_VERSION="${ETL_VERSION}"
)

# compile options
target_compile_options(etl PUBLIC
    -Wno-literal-suffix
)

# installation setup
install(TARGETS etl
    EXPORT etlTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
install(EXPORT etlTargets DESTINATION lib/cmake/etl)

# config version
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
