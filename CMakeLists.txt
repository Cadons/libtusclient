cmake_minimum_required(VERSION 3.15)
project(${CONAN_NAME} CXX)
set(CMAKE_CXX_STANDARD 17)
set(PROJECT_VERSION ${CONAN_VERSION})
set(PROJECT_LICENSE ${CONAN_PACKAGE_LICENSE})
set(OUTPUT_BASE_DIR ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

configure_file(config.h.in config.h)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)
find_package(CURL REQUIRED)
find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(easyloggingpp REQUIRED)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} CURL::libcurl Boost::boost nlohmann_json::nlohmann_json easyloggingpp::easyloggingpp)

target_include_directories(libtusclient 
    PUBLIC 
        include     
    PRIVATE 
        ${CMAKE_CURRENT_BINARY_DIR})



set_target_properties(libtusclient PROPERTIES PUBLIC_HEADER "include/libtusclient.h")
install(TARGETS libtusclient
        EXPORT libtusclient
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)


enable_testing()

add_subdirectory(test)