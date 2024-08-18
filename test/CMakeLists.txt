find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
if (NOT GTEST_FOUND)
    message(FATAL_ERROR "Cannot find gtest")
endif()

enable_testing()

file(GLOB TEST_SRCS *.*)
add_executable(test_all ${TEST_SRCS})

target_include_directories(test_all PRIVATE
	${CMAKE_HOME_DIRECTORY}/include
	${GTEST_SOURCE_DIR}/include ${GTEST_SOURCE_DIR}
)

target_compile_options(test_all PRIVATE 
    -g 
    -Wall -Wextra 
    -Wno-literal-suffix 
    -Wno-missing-field-initializers
)

target_link_libraries(test_all PRIVATE
	m
	Threads::Threads
	${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES}
)
