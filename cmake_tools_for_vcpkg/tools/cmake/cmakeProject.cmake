# Load the external configuration file
if(POLICY CMP0177)
    cmake_policy(SET CMP0177 NEW)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/../../../cmake/config.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/targetCreator.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deploy.cmake)

# Function to set the project name
function(set_project_name)
    if(DEFINED MY_CMAKE_PROJECT_NAME)
        set(CMAKE_PROJECT_NAME ${MY_CMAKE_PROJECT_NAME} PARENT_SCOPE)
        message(STATUS "Project name set to: ${CMAKE_PROJECT_NAME}")
    else()
        message(FATAL_ERROR "Project not configured, use setup.py in the tools folder. Add name to vcpkg.json")
    endif()
endfunction()

# Function to display project information
function(print_project_info)
    message("-----------------CMake Project-------------------------")
    message("Project Name: ${MY_CMAKE_PROJECT_NAME}")
    message("Version Semver: ${MY_CMAKE_PROJECT_SEMVER}")
    message("Version: ${MY_CMAKE_PROJECT_VERSION}")
    message("------------------------------------------")
endfunction()

# Function to configure MSVC hot reload
function(configure_msvc_hot_reload)
    if (POLICY CMP0141)
        cmake_policy(SET CMP0141 NEW)
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
            "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
    endif()
endfunction()

# Function to set project properties
function(set_project_properties)
    set(PROJECT_VERSION "${MY_CMAKE_PROJECT_VERSION}" PARENT_SCOPE)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PROJECT_LABEL "${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_VERSION}")
    set(CMAKE_CXX_STANDARD 20 PARENT_SCOPE)
    set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
endfunction()

# Function to configure output directories
function(configure_output_directories)
    set(ARTIFACT_FOLDER "${CMAKE_BINARY_DIR}/Packages" PARENT_SCOPE)
    set(PROJECT_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}" PARENT_SCOPE)
    set(PROJECT_VCPKG_INSTALLED_ROOT "${CMAKE_BINARY_DIR}/vcpkg_installed" PARENT_SCOPE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/bin" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib" PARENT_SCOPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/lib" PARENT_SCOPE)
endfunction()

# Function to collect source and header files
function(collect_source_files)
    file(GLOB_RECURSE Lib_SRCS "${CMAKE_CURRENT_LIST_DIR}/../../../src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/../../../src/*.cxx")
    file(GLOB_RECURSE Lib_HDRS "${CMAKE_CURRENT_LIST_DIR}/../../../include/*.h" "${CMAKE_CURRENT_LIST_DIR}/../../../include/*.hpp")
    set(SOURCES ${Lib_SRCS} ${Lib_HDRS} PARENT_SCOPE)
endfunction()

# Call functions in sequence
set_project_name()
set_project_properties()
print_project_info()
configure_msvc_hot_reload()

configure_output_directories()
collect_source_files()

# Debugging: print project name and source files
message("Configuring Qt project.....")
message("Project Name: ${CMAKE_PROJECT_NAME}")
