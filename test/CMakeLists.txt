cmake_minimum_required(VERSION 3.12)

message(STATUS "${PROJECT_NAME} Unit Test")

# Recursively find all .cpp and .h files in the current directory
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.h")
#find_package here--------------------

find_package(GTest REQUIRED)#GoogleTest package inclusion
find_package(libzippp REQUIRED)
#remove_definitions(-DBUILD_SHARED)#this is IMPORTANT if you are exporting symbols with macro in particular if you have QObjects
#-------------------------------------
#linking here--------------------
#set(TEST_LIBRARY_LINKING "")#comment this if you are using GoogleTest
set(TEST_LIBRARY_LINKING GTest::GTest GTest::gmock libzippp::libzippp)#for GoogleTest
#-------------------------------------
# Create the executable for the tests
add_executable(${PROJECT_LIB}_test ${TEST_SOURCES})

# Link the libraries
target_link_libraries(${PROJECT_LIB}_test PRIVATE ${PROJECT_LIB} ${TEST_LIBRARY_LINKING})

# Include directories for the project (if needed)
target_include_directories(${PROJECT_LIB}_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})

#Include GoogleTest and setup tests

#GoogleTest configuration
include(GoogleTest)
gtest_discover_tests(${PROJECT_LIB}_test)

#Configuration for other test, this should need if you are using gtest
#add_test(NAME ${PROJECT_NAME}Tests COMMAND ${PROJECT_NAME}_test)

#other options here--------------------
