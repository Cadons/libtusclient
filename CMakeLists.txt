cmake_minimum_required(VERSION 3.15)
project(libtusclient CXX)

set(OUTPUT_BASE_DIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/lib)

add_library(libtusclient src/libtusclient.cpp)
target_include_directories(libtusclient PUBLIC include)



set_target_properties(libtusclient PROPERTIES PUBLIC_HEADER "include/libtusclient.h")
install(TARGETS libtusclient)
