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
function (configure_compiler cpp_standard)
    if(MSVC)
        # Add MSVC-specific compile options.
        add_compile_options(/W4 /WX)
        if (POLICY CMP0141)
            # Set policy for MSVC debug information format.
            cmake_policy(SET CMP0141 NEW)
            set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
                    "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
        endif ()
    else()
        # Add general compile options for non-MSVC compilers.
        #add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()

    # Set the C++ standard and enforce strict compliance.
    set(CMAKE_CXX_STANDARD ${cpp_standard} PARENT_SCOPE)
    set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
    set(CMAKE_CXX_EXTENSIONS OFF PARENT_SCOPE)
endfunction()
