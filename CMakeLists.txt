cmake_minimum_required(VERSION 3.14)

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

# build interface library
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

# target: install
install(TARGETS etl
    EXPORT etlTargets
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

# config version
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/etlConfigVersion.cmake"
    VERSION ${ETL_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# config file
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/etlConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/etlConfig.cmake"
    INSTALL_DESTINATION lib/cmake/etl
)

# install config
install(EXPORT etlTargets DESTINATION lib/cmake/etl)
install(
    FILES "${PROJECT_BINARY_DIR}/etlConfigVersion.cmake"
          "${PROJECT_BINARY_DIR}/etlConfig.cmake"
    DESTINATION lib/cmake/etl
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

# target: uninstall
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

# create test
function(etl_create_test cpp_version)
    file(GLOB TEST_SRCS test/*.*)
    add_executable(test_${cpp_version} ${TEST_SRCS})

    target_compile_features(test_${cpp_version} PRIVATE cxx_std_${cpp_version})
    target_link_libraries(test_${cpp_version} PRIVATE
        m
        Threads::Threads
        etl
        gtest
        gtest_main
    )
    target_compile_options(test_${cpp_version} PRIVATE 
        -g 
        -Wall -Wextra 
        -Wno-literal-suffix 
        -Wno-missing-field-initializers
    )
endfunction()

# build test
if (BUILD_TESTS)
    find_package(Threads REQUIRED)
    find_package(GTest QUIET)

    if (NOT GTest_FOUND)
        message(STATUS "Fetching GTest...")
        include(FetchContent)
        FetchContent_Declare(
            GTest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        main
        )
        FetchContent_MakeAvailable(GTest)
    endif()

    etl_create_test(17)
    etl_create_test(20)
    etl_create_test(23)

    enable_testing()
    add_test(NAME test_17 COMMAND test_17)
    add_test(NAME test_20 COMMAND test_20)
    add_test(NAME test_23 COMMAND test_23)
endif()

# build docs
if (BUILD_DOCS)
    add_subdirectory(docs)
endif()
