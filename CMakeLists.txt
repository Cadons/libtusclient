list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Common OPTIONAL)
get_version(${CMAKE_CURRENT_LIST_DIR}/project.json PROJECT_VERSION)
message(STATUS "tusclient by Cadons: ${PROJECT_VERSION}")
cmake_minimum_required(VERSION 3.23)
#shared library option
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
project(tusclient
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "A monorepo project"
        LANGUAGES CXX
)

#import dependencies
find_package(CURL REQUIRED)
find_package(boost_lexical_cast CONFIG REQUIRED)
find_package(boost_uuid CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(glog REQUIRED)

# Organization info
set(PROJECT_ORG_NAME "Cadons")
set(PROJECT_ORG_URL "https://example.com")

# Prefer Ninja if not explicitly specified

if (NOT CMAKE_GENERATOR)
    message(STATUS "No generator specified. Consider using Ninja for faster builds.")
endif ()


# Set C++ standard
configure_compiler(20)

# Default build type
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)
endif ()

# Enable IDE folder grouping
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add extra cmake modules (optional)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Common OPTIONAL)
include(QUtils OPTIONAL)

# Testing
include(CTest)
enable_testing()

# Version header generation
configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/version.h.in
        ${CMAKE_BINARY_DIR}/generated/version.h
        @ONLY
)

# === Add subdirectories ===
add_subdirectory(lib)
