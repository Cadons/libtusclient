cmake_minimum_required(VERSION 3.15)
project(libtusclient CXX)




add_library(libtusclient src/libtusclient.cpp)
target_include_directories(libtusclient PUBLIC include)



set_target_properties(libtusclient PROPERTIES PUBLIC_HEADER "include/libtusclient.h")
install(TARGETS libtusclient)
