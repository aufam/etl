include("../cmake/CPM.cmake")

find_package(Threads REQUIRED)
CPMAddPackage("gh:google/googletest@1.15.2")

file(GLOB TEST_SRCS *.*)

function(etl_create_test cpp_version)
    add_executable(test_${cpp_version} ${TEST_SRCS})

    target_compile_features(test_${cpp_version} PRIVATE cxx_std_${cpp_version})

    target_link_libraries(test_${cpp_version} PRIVATE
        etl
        Threads::Threads
        gtest
        gtest_main
    )

    target_compile_options(test_${cpp_version} PRIVATE
        -g
        -Wall
        -Wextra
        -Wno-missing-field-initializers
    )
endfunction()

etl_create_test(17)
etl_create_test(20)
etl_create_test(23)

enable_testing()
add_test(NAME test_17 COMMAND test_17)
add_test(NAME test_20 COMMAND test_20)
add_test(NAME test_23 COMMAND test_23)
