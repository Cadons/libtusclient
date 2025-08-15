function(add_program_icon target icon_path)
    if (WIN32)
       #todo: implement
    elseif(APPLE)
        #todo: implement

    elseif(UNIX AND NOT APPLE)
        #todo: implement

    endif()
endfunction()

function (auto_deploy_libraries target_name)
    if (WIN32)
        #todo: implement

    endif ()
    if (APPLE)
        #todo: implement

    endif ()
    if (UNIX AND NOT APPLE)
        #todo: implement

    endif ()
endfunction()

function(create_i18n_language target_name language)
    #todo: implement
endfunction()