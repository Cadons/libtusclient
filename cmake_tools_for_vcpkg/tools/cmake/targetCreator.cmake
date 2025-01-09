include(GNUInstallDirs)
include(${CMAKE_CURRENT_LIST_DIR}/Qt6Deployer.cmake)
# Function for executables
function(create_executable target_name is_qt_project)
    if(${is_qt_project})
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTORCC ON)
        set(CMAKE_AUTOUIC ON)
        QT6_ADD_RESOURCES(SOURCES ${RESOURCES})
        QT6_ADD_EXECUTABLE(${target_name} ${SOURCES})
    else()
        add_executable(${target_name} ${SOURCES})
    endif()

    target_link_libraries(${target_name} PRIVATE ${PROJECT_LIBRARIES})
    target_include_directories(${target_name} PUBLIC 
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
    if(${is_qt_project})
        deploy_qt6_dependencies(${target_name})  
    endif()
endfunction()

# Function for dynamic libraries
function(create_shared_library target_name is_qt_project)   
if(WIN32)
        add_compile_definitions(BUILD_SHARED)

        if(DEFINED EXPORT_ALL AND EXPORT_ALL)
            message("All symbols will be exported")
            add_compile_definitions(EXPORT_ALL)
            set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
        endif()
    endif()
    if(${is_qt_project})
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTORCC ON)
        set(CMAKE_AUTOUIC ON)        
        QT6_ADD_RESOURCES(SOURCES ${RESOURCES})
        QT6_ADD_LIBRARY(${target_name} SHARED ${SOURCES})
    else()
        add_library(${target_name} SHARED ${SOURCES})
    endif()
    
    target_link_libraries(${target_name} PRIVATE ${PRIVATE_PROJECT_LIBRARIES})
    target_link_libraries(${target_name} PUBLIC ${PROJECT_LIBRARIES})
    target_link_libraries(${target_name} INTERFACE ${INTERFACE_PROJECT_LIBRARIES})   
    target_include_directories(${target_name} PUBLIC 
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
    if(${is_qt_project})
        deploy_qt6_dependencies(${target_name})  
    endif()
endfunction()

# Function for static libraries
function(create_static_library target_name is_qt_project)
    if(${is_qt_project})
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTORCC ON)
        set(CMAKE_AUTOUIC ON)
        QT6_ADD_RESOURCES(SOURCES ${RESOURCES})
        QT6_ADD_LIBRARY(${target_name} STATIC ${SOURCES})
    else()
        add_library(${target_name} STATIC ${SOURCES})
    endif()

    target_link_libraries(${target_name} PRIVATE ${PRIVATE_PROJECT_LIBRARIES})
    target_link_libraries(${target_name} PUBLIC ${PROJECT_LIBRARIES})
    target_link_libraries(${target_name} INTERFACE ${INTERFACE_PROJECT_LIBRARIES})   
    target_include_directories(${target_name} PUBLIC 
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )
    if(${is_qt_project})
        deploy_qt6_dependencies(${target_name})  
    endif()
endfunction()
