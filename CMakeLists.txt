# ------------------------CMake Project Creator------------------------
#                     
#                   Make C++ Faster with VCPKG and CMake
#                   -------------------------------------
# ---------------------------------------------------------------------
# 

cmake_minimum_required(VERSION 3.12)
project(libtusclient CXX)

include(${CMAKE_CURRENT_LIST_DIR}/cmake_tools_for_vcpkg/tools/cmake/cmakeProject.cmake)

message(STATUS "${PROJECT_NAME} Build")

# Find required packages

# Configure file for headers
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/config.h.in ${CMAKE_CURRENT_LIST_DIR}/include/config.h)
find_package(CURL REQUIRED)
find_package(boost_lexical_cast CONFIG REQUIRED)
find_package(boost_uuid CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(glog REQUIRED)

set(PROJECT_LIBRARIES  CURL::libcurl Boost::uuid Boost::lexical_cast nlohmann_json::nlohmann_json glog::glog)
set(EXPORT_ALL 1)
create_shared_library("libtusclient" false)

setup_install(${CMAKE_PROJECT_NAME})
setup_package(${CMAKE_PROJECT_NAME})
# Option to build tests
option(BUILD_TEST "Build the test suite" ON)

if(BUILD_TEST)
    # Enable testing
    enable_testing()


    set(PROJECT_LIB "libtusclient")
    add_subdirectory(test)
endif()
