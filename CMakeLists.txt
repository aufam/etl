cmake_minimum_required(VERSION 3.7)

# project config
file(READ version.txt ETL_VERSION)
project(etl VERSION ${ETL_VERSION})

# some options
option(BUILD_TESTS "Build unit tests using Google Test" OFF)
option(BUILD_DOCS "Build docs using Doxygen" OFF)

# some messages
message("ETL_VERSION : ${ETL_VERSION}")
message("BUILD_TESTS : ${BUILD_TESTS}")
message("BUILD_DOCS  : ${BUILD_DOCS}")

# build library
add_library(etl INTERFACE)

target_include_directories(etl INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_compile_options(etl INTERFACE 
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>)

target_compile_definitions(etl INTERFACE 
        -DETL_VERSION="${PROJECT_VERSION}"
        -DETL_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        -DETL_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        -DETL_VERSION_PATCH=${PROJECT_VERSION_PATCH})

# install target
install(TARGETS etl
        EXPORT etlTargets
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development
        RUNTIME DESTINATION bin COMPONENT Runtime
        PUBLIC_HEADER DESTINATION include COMPONENT Development
        BUNDLE DESTINATION bin COMPONENT Runtime)

# config version
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/etlConfigVersion.cmake"
        VERSION ${ETL_VERSION}
        COMPATIBILITY AnyNewerVersion)

# config file
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/etlConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/etlConfig.cmake"
        INSTALL_DESTINATION lib/cmake/etl)

# install config
install(EXPORT etlTargets DESTINATION lib/cmake/etl)
install(FILES "${PROJECT_BINARY_DIR}/etlConfigVersion.cmake"
        "${PROJECT_BINARY_DIR}/etlConfig.cmake"
        DESTINATION lib/cmake/etl)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

# uninstall all
if(NOT TARGET uninstall)
    configure_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
            IMMEDIATE @ONLY)

    add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
endif()

# build test and docs
if (BUILD_TESTS)
    add_subdirectory(test)
endif()
if (BUILD_DOCS)
    add_subdirectory(docs)
endif()
