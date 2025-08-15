function(get_json_value MANIFEST_JSON KEY OUT_VALUE)
    # Check if the manifest file exists
    file(READ ${MANIFEST_JSON} JSON_CONTENT)
    # Extract value for the specified key
    string(JSON VALUE GET "${JSON_CONTENT}" ${KEY})
    # Set the output variable to the extracted value
    set(${OUT_VALUE} "${VALUE}" PARENT_SCOPE)
endfunction()
#get version from manifest.json
function(get_version MANIFEST_JSON OUT_VERSION)
    # Check if the manifest file exists
    get_json_value(${MANIFEST_JSON} "version" VERSION)
    # Set the output variable to the extracted version
    set(${OUT_VERSION} "${VERSION}" PARENT_SCOPE)
endfunction()

function(get_organization MANIFEST_JSON OUT_ORG)
    get_json_value(${MANIFEST_JSON} "organization" ORG)
    # Check if the organization key exists in the JSON
    set(${OUT_ORG} "${ORG}" PARENT_SCOPE)
endfunction()

function(get_name MANIFEST_JSON OUT_NAME)
    # Check if the manifest file exists
    get_json_value(${MANIFEST_JSON} "name" NAME)
    # Set the output variable to the extracted name
    set(${OUT_NAME} "${NAME}" PARENT_SCOPE)
endfunction()

function(show_module_info MANIFEST_JSON)
    # Get the module name
    get_name(${MANIFEST_JSON} MODULE_NAME)
    # Get the module version
    get_version(${MANIFEST_JSON} MODULE_VERSION)
    # Display the module information
    message(STATUS "Module: ${MODULE_NAME}, Version: ${MODULE_VERSION}")
endfunction()



# Function to configure build directories for a specific module.
# Arguments:
#   module_name - The name of the module for which the build directories are configured.
# Example usage: configure_build(my_module)
function(configure_build module_name)
    # Set the base output directory for the module.
    set(MODULE_OUTPUT_BASE "${CMAKE_BINARY_DIR}/build/${module_name}")

    # Set runtime, library, and archive output directories for the module.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MODULE_OUTPUT_BASE}/bin" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MODULE_OUTPUT_BASE}/lib" PARENT_SCOPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MODULE_OUTPUT_BASE}/lib" PARENT_SCOPE)

    # Create the necessary directories for binaries and libraries.
    file(MAKE_DIRECTORY "${MODULE_OUTPUT_BASE}/bin")
    file(MAKE_DIRECTORY "${MODULE_OUTPUT_BASE}/lib")
    source_group("Headers"   REGULAR_EXPRESSION "\\.(h|hh|hpp|ipp)$")
    source_group("Sources"   REGULAR_EXPRESSION "\\.(c|cc|cxx|cpp|m|mm)$")
    source_group("QML"       REGULAR_EXPRESSION "\\.(qml|qmltypes)$")
    source_group("Resources" REGULAR_EXPRESSION "\\.(qrc|ui|qss|qm)$")
    source_group("Images"    REGULAR_EXPRESSION "\\.(png|jpg|jpeg|gif|bmp)$")
    source_group("Icons"     REGULAR_EXPRESSION "\\.(ico|icns)$")
    source_group("Misc"      REGULAR_EXPRESSION "\\.(txt|md|json|xml|yaml|yml)$")
    source_group("CMake"     REGULAR_EXPRESSION "\\.(cmake|cmake.in)$")
endfunction()

# Function to configure installation rules for a target.
# Arguments:
#   target_name - The name of the target to configure installation for.
# Example usage: configure_install(my_target)
function(configure_install target_name)
    # Install the target's runtime, library, and archive files to their respective destinations.
    install(TARGETS ${target_name}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib)

    # Install header files from the include directory to the installation include directory.
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
            DESTINATION include
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
endfunction()

# Function to configure compiler settings.
# Arguments:
#   cpp_standard - The C++ standard to use (e.g., 11, 14, 17, etc.).
# Example usage: configure_compiler(17)

function(configure_compiler cpp_standard)
    # Set C++ standard requirements
    set(CMAKE_CXX_STANDARD ${cpp_standard})
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    if(MSVC)
        # MSVC specific settings
        add_compile_options(
                /permissive-   # Strict standards compliance
                /Zc:__cplusplus # Correct __cplusplus macro
                /Zc:preprocessor # Standards-compliant preprocessor
                /std:c++latest  # Enable latest C++ features
        )
        add_compile_options(/MP) # Enable multi-processor compilation
        # Debug information format configuration
        if(POLICY CMP0141)
            cmake_policy(SET CMP0141 NEW)
            set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
                    "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,
                    $<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,
                    $<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
        endif()
    else()
            add_compile_options(-std=c++${cpp_standard})
    endif()

    # Default build type
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING
                "Build type (Debug/Release/RelWithDebInfo/MinSizeRel)" FORCE)
    endif()

    # IDE folder organization
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endfunction()