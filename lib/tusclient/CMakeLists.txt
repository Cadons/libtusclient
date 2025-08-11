# Include common cmake scripts
include(${CMAKE_CURRENT_SOURCE_DIR}/.cmake/sources.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)

# Configure build output directories for the target
configure_build(tusclient)

# Create executable target
add_library(tusclient SHARED ${TUSCLIENT_SOURCES} ${TUSCLIENT_HEADERS} ${TUSCLIENT_RESOURCES})

# Include headers directory
target_include_directories(tusclient PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/tusclient>
        $<INSTALL_INTERFACE:include/tusclient>)

# Link libraries
target_link_libraries(tusclient PUBLIC CURL::libcurl Boost::uuid Boost::lexical_cast nlohmann_json::nlohmann_json glog::glog)
target_compile_definitions(tusclient PRIVATE TUSCLIENT_EXPORTS)
# Enable testing if tests exist
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
# Install target
configure_install(tusclient)