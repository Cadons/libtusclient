include(${CMAKE_CURRENT_LIST_DIR}/Qt6Deployer.cmake)
# Include GNU install directories for standard installation locations
include(GNUInstallDirs)

# Function for installation
function(setup_install project_target)
    # Deployment section for Qt dependencies
    if(Qt6)
        deploy_qt6_dependencies(${project_target})  
    endif()

    # Install settings
    install(TARGETS ${project_target}
        EXPORT ${project_target}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${project_target}
    )

    # Install header files
    if(EXISTS "${EXPORTED_HEADER}")
        install(FILES ${EXPORTED_HEADER} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${project_target})
    else()
        install(DIRECTORY include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${project_target}
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*_p.h" EXCLUDE
        )
    endif()

    # Install project_target export file
    install(EXPORT ${project_target}Targets
            FILE ${project_target}Targets.cmake
            NAMESPACE ${project_target}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/../share/${project_target}
    )

    # Generate and install Config file for package
    include(CMakePackageConfigHelpers)
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${project_target}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/../share/${project_target}
    )

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${project_target}Config.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/../share/${project_target}
    )

    # Install additional files from the build directory
    if(EXISTS ${CMAKE_BINARY_DIR}/$<CONFIG>/bin)
        install(DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>/bin/
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            FILES_MATCHING PATTERN "*"
        )
    endif()
endfunction()

# Function for packaging
function(setup_package project_target)
    # Set the output directory for the package
    set(CPACK_OUTPUT_DIRECTORY "${ARTIFACT_FOLDER}")
    set(CPACK_PACKAGE_NAME "${project_target}")
    include(CPack)
endfunction()

