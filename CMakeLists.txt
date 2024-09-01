cmake_minimum_required(VERSION 3.15)
project(${CONAN_NAME} CXX)

set(PROJECT_VERSION ${CONAN_VERSION})
set(OUTPUT_BASE_DIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/lib)

add_library(libtusclient src/libtusclient.cpp)
target_include_directories(libtusclient PUBLIC include)



set_target_properties(libtusclient PROPERTIES PUBLIC_HEADER "include/libtusclient.h")
install(TARGETS libtusclient)
message(${PROJECT_NAME} " - " ${PROJECT_VERSION})