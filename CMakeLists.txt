cmake_minimum_required(VERSION 3.15)
project(${CONAN_NAME} CXX)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_VERSION ${CONAN_VERSION})
set(PROJECT_LICENSE ${CONAN_PACKAGE_LICENSE})
set(OUTPUT_BASE_DIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

configure_file(config.h.in config.h)

add_library(libtusclient SHARED src/libtusclient.cpp)

target_include_directories(libtusclient 
    PUBLIC 
        include     
    PRIVATE 
        ${CMAKE_CURRENT_BINARY_DIR})



set_target_properties(libtusclient PROPERTIES PUBLIC_HEADER "include/libtusclient.h")
install(TARGETS libtusclient)
message("You are working on: "+${PROJECT_NAME} " - " ${CONAN_VERSION})



enable_testing()

add_subdirectory(test)