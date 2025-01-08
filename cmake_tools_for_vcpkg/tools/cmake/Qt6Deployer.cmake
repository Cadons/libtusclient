function(deploy_qt6_dependencies target_name)
    if(WIN32)
        # Configura il deployment tool e la directory QML in base alla configurazione
        set(DEPLOYMENT_TOOL "$<$<CONFIG:Debug>:.debug.bat>$<$<CONFIG:Release>:.exe>")
        set(QML_DIR "$<$<CONFIG:Debug>:debug/Qt6/qml>$<$<CONFIG:Release>:Qt6/qml>")

        # Percorso per windeployqt in base alla configurazione
        set(WINDEPLOYQT6_PATH "${PROJECT_VCPKG_INSTALLED_ROOT}/x64-windows/tools/Qt6/bin/windeployqt${DEPLOYMENT_TOOL}")
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt dependencies for $<CONFIG> configuration of ${target_name}..."
                COMMAND cmd /c "${WINDEPLOYQT6_PATH} --force-openssl --qmldir \"${Qt6_DIR}/../../${QML_DIR}\" $<TARGET_FILE:${target_name}> >nul 2>&1 || exit 0"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMAND_EXPAND_LISTS
                VERBATIM
            )
    endif()
endfunction()
