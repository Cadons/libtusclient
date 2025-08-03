include(${CMAKE_CURRENT_SOURCE_DIR}/../.cmake/sources.cmake)
#Find GoogleTest package
find_package(GTest REQUIRED)

configure_build(tusclient_test)
find_package(GTest REQUIRED)#GoogleTest package inclusion
find_package(libzippp REQUIRED)
# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include/
)

# Add test executable

# Use the modified copies
add_executable(tusclient_test ${TUSCLIENT_TEST_SOURCES})
# Link libraries
target_link_libraries(tusclient_test PRIVATE GTest::GTest GTest::gmock libzippp::libzippp tusclient)

#auto discover tests
gtest_discover_tests(tusclient_test
    DISCOVERY_TIMEOUT 60
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
