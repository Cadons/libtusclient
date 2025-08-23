# Include common cmake scripts
include(${CMAKE_CURRENT_SOURCE_DIR}/.cmake/sources.cmake)
show_module_info(${CMAKE_CURRENT_LIST_DIR}/package.json)

# Configure build output directories for the target
configure_build(tusclient)

# Create executable target
if(BUILD_SHARED_LIBS)
add_library(tusclient SHARED ${TUSCLIENT_SOURCES} ${TUSCLIENT_HEADERS} ${TUSCLIENT_RESOURCES})
else()
    add_library(tusclient STATIC ${TUSCLIENT_SOURCES} ${TUSCLIENT_HEADERS} ${TUSCLIENT_RESOURCES})
endif()

# Include headers directory
target_include_directories(tusclient PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/tusclient>
        $<INSTALL_INTERFACE:include/tusclient>)

# Link libraries
target_link_libraries(tusclient PUBLIC CURL::libcurl Boost::uuid Boost::lexical_cast nlohmann_json::nlohmann_json glog::glog fmt::fmt)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(tusclient PRIVATE TUSCLIENT_EXPORTS)
    target_compile_definitions(tusclient PRIVATE TUSCLIENT_SHARED)
endif ()
# Enable testing if tests exist
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
# Install target
configure_install(tusclient)
#For destribution, we need to create a config file for the package
include(CMakePackageConfigHelpers)
configure_package_config_file(
        "${CMAKE_SOURCE_DIR}/cmake/tusclientConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/tusclientConfig.cmake"
        INSTALL_DESTINATION lib/cmake/tusclient
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tusclientConfig.cmake"
        DESTINATION lib/cmake/tusclient)