cmake_minimum_required(VERSION 3.14)

project(etl
    VERSION     1.0.0
    DESCRIPTION "Embedded Template Library"
)

option(ETL_BUILD_TESTS "Build unit tests using Google Test" OFF)
option(ETL_BUILD_DOCS "Build docs using Doxygen" OFF)

message(STATUS "ETL_VERSION     : ${ETL_VERSION}")
message(STATUS "ETL_BUILD_TESTS : ${ETL_BUILD_TESTS}")
message(STATUS "ETL_BUILD_DOCS  : ${ETL_BUILD_DOCS}")

add_library(etl INTERFACE)

target_include_directories(etl INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(etl INTERFACE
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>
)

target_compile_definitions(etl INTERFACE
    -DETL_VERSION="${PROJECT_VERSION}"
    -DETL_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    -DETL_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    -DETL_VERSION_PATCH=${PROJECT_VERSION_PATCH}
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

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/etlConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/etlConfig.cmake"
    INSTALL_DESTINATION lib/cmake/etl
)

install(
    FILES "${PROJECT_BINARY_DIR}/etlConfigVersion.cmake"
          "${PROJECT_BINARY_DIR}/etlConfig.cmake"
    DESTINATION lib/cmake/etl
)

if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake
    )
endif()

if (ETL_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (ETL_BUILD_DOCS)
    add_subdirectory(docs)
endif()
