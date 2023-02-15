find_package(Threads REQUIRED)
find_package(GTest)

if (GTEST_FOUND)
    set(CMAKE_CXX_STANDARD 17)

    file(GLOB TEST_SRCS *.*)
    add_executable(${PROJECT_NAME} ${TEST_SRCS})

    target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/include/gtest/)

    target_link_libraries(${PROJECT_NAME} PRIVATE
            m
            Threads::Threads
            /usr/local/lib/libgtest.a)
endif(GTEST_FOUND)