# This file is part of the Qt Utils project, which provides utility functions for CMake projects using Qt.

# Function to add an icon to the program.
# Arguments:
#   target - The name of the target to which the icon should be added.
#   icon_path - The path to the directory containing the icon files.
#   icon_name - The name of the icon file (without extension). Defaults to "app_icon".
# Example usage: add_program_icon(my_target "${CMAKE_SOURCE_DIR}/resources/images" "app_icon")
function(add_program_icon target icon_path icon_name)
    # Default icon_name to "app_icon" if not provided
    if(NOT icon_name)
        set(icon_name "app_icon")
    endif()

    if(WIN32)
        # Windows: inject the .rc resource file
        set(windows_rc "${icon_path}/${icon_name}.rc")
        if(EXISTS "${windows_rc}")
            target_sources(${target} PRIVATE "${windows_rc}")
        else()
            message(WARNING "add_program_icon: ${windows_rc} not found")
        endif()
    endif()

    if(APPLE)
        # macOS: bundle the .icns and set the bundle property
        set(icns_file "${icon_path}/${icon_name}.icns")
        if(EXISTS "${icns_file}")
            # Tell CMake to include it in the .app bundle Resources
            set_source_files_properties(
                    "${icns_file}"
                    PROPERTIES MACOSX_PACKAGE_LOCATION Resources
            )
            target_sources(${target} PRIVATE "${icns_file}")

            # And set the bundle icon for the target
            set_target_properties(
                    ${target}
                    PROPERTIES MACOSX_BUNDLE_ICON_FILE "${icon_name}.icns"
            )
        else()
            message(WARNING "add_program_icon: ${icns_file} not found")
        endif()
    endif()
endfunction()

# Function to automatically deploy Qt for a specific target.
# Arguments:
#   target_name - The name of the target for which to deploy Qt.
# Example usage: auto_deploy_qt(my_target)
function(auto_deploy_qt target_name)
    if(WIN32)
        # Set deployment tool and QML directory for Windows.
        set(DEPLOYMENT_TOOL "$<$<CONFIG:Debug>:windeployqt.debug.bat>$<$<CONFIG:Release>:windeployqt.exe>")
        set(QML_DIR "${Qt6_DIR}/../../$<$<CONFIG:Debug>:debug/Qt6/qml>$<$<CONFIG:Release>:Qt6/qml>")

        # Check if QML directory exists.
        if(NOT EXISTS "${QML_DIR}")
            set (QML_DIR "None") # Fallback if the directory does not exist, no qml needs to be deployed
        endif()

        # Set variables for build path and target file.
        set(BUILD_ROOT "${CMAKE_BINARY_DIR}")
        set(TARGET_FILE_PATH $<TARGET_FILE:${target_name}>)

        # Normalize paths for Windows
        file(TO_NATIVE_PATH "${TARGET_FILE_PATH}" TARGET_FILE_PATH)
        file(TO_NATIVE_PATH "${BUILD_ROOT}" BUILD_ROOT)
        file(TO_NATIVE_PATH "${QML_DIR}" QML_DIR)
        file(TO_NATIVE_PATH "${DEPLOYMENT_TOOL}" DEPLOYMENT_TOOL)

        # Add custom command to deploy Qt after the target is built.
        add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt for ${target_name}"
                COMMAND cmd /c call "${CMAKE_SOURCE_DIR}/cmake/scripts/DeployWindowsQtForVcpkg.bat" "${BUILD_ROOT}" "${DEPLOYMENT_TOOL}" "${QML_DIR}" "${TARGET_FILE_PATH}"
                VERBATIM
        )
    endif()
endfunction()