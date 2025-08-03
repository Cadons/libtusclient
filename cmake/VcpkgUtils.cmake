
# Function to deploy a shared library from vcpkg.
# Arguments:
#   TARGET_NAME - The name of the target that depends on the shared library.
#   FILENAME - The name of the shared library file to deploy.
# Example usage: deploy_vcpkg_shared_lib(my_target "libssl-x64.dll")
function(deploy_vcpkg_shared_lib TARGET_NAME FILENAME)
    # Initialize variables for architecture, OS, and library details.
    set(ARCH "none")
    set(OS_NAME "none")
    set(TRIPLET_POSTFIX "-dynamic")
    set(LIB_EXTENSION "none")
    set(SHARED_LIB_FOLDER "none")
    set(LIB_PREFIX "none")
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" PROCESSOR_LOWERCASE)

    # Determine the architecture based on the processor type.
    if (PROCESSOR_LOWERCASE MATCHES "x86_64|amd64")
        set(ARCH "x64")
    elseif (PROCESSOR_LOWERCASE MATCHES "armv7l|armv8l|arm") # ARM 32-bit
        set(ARCH "arm")
    elseif (PROCESSOR_LOWERCASE MATCHES "aarch64|arm64") # ARM 64-bit
        set(ARCH "arm64")
    else()
        # Fail if the architecture is unsupported.
        message(FATAL_ERROR "Architecture not supported: ${CMAKE_SYSTEM_PROCESSOR}")
        return()
    endif()

    # Configure settings for Windows.
    if(WIN32)
        set(OS_NAME "windows")
        set(TRIPLET_POSTFIX "")
        set(LIB_EXTENSION "dll")
        set(SHARED_LIB_FOLDER "bin")
        set(LIB_PREFIX "")
    else ()
        # Exit if the OS is not Windows.
        return()
    endif ()

    # Log the triplet being used.
    message(STATUS "Triplet: ${ARCH}-${OS_NAME}${TRIPLET_POSTFIX}")

    # Define the source file path for the shared library.
    set(SOURCE_FILE_PATH "$<IF:$<CONFIG:Debug>,${PROJECT_VCPKG_INSTALLED_ROOT}/${ARCH}-${OS_NAME}${TRIPLET_POSTFIX}/debug/${SHARED_LIB_FOLDER}/${LIB_PREFIX}${FILENAME}.${LIB_EXTENSION},${PROJECT_VCPKG_INSTALLED_ROOT}/${ARCH}-${OS_NAME}${TRIPLET_POSTFIX}/${SHARED_LIB_FOLDER}/${LIB_PREFIX}${FILENAME}.${LIB_EXTENSION}>")

    # Get the target type (e.g., executable, library).
    get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)

    # Define the destination path for the shared library.
    if(WIN32)
        set(DESTINATION_PATH "${CMAKE_BINARY_DIR}/$<IF:$<CONFIG:Debug>,Debug,Release>/${SHARED_LIB_FOLDER}/${LIB_PREFIX}${FILENAME}.${LIB_EXTENSION}")
    else()
        set(DESTINATION_PATH "${CMAKE_BINARY_DIR}/${SHARED_LIB_FOLDER}/${LIB_PREFIX}${FILENAME}.${LIB_EXTENSION}")
    endif()

    # Add a custom command to copy the shared library after the target is built.
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying library: ${SOURCE_FILE_PATH} -> ${DESTINATION_PATH}"
            COMMAND ${CMAKE_COMMAND} -E copy "${SOURCE_FILE_PATH}" "${DESTINATION_PATH}"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            VERBATIM
    )
endfunction()