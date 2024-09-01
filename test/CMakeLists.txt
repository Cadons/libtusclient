cmake_minimum_required(VERSION 3.15)
find_package(GTest REQUIRED)

set(PROJECT_NAME_TEST ${PROJECT_NAME}_test)

file(GLOB_RECURSE TEST_SOURCES *.cpp)

add_executable(${PROJECT_NAME_TEST} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME_TEST} gtest::gtest GTest::Main ${PROJECT_NAME})

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME_TEST})
