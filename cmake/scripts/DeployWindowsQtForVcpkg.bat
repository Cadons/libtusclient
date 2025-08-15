@echo off
setlocal EnableDelayedExpansion

if "%~1"=="" (
    echo Usage: %~nx0 ^<build_directory^> ^<windeployqt_exe^> ^<target_name^>  ^<qml_dir^> ^<target_file_path^>
    exit /b 1
)
set "BUILD_DIR=%~1"
set "LOCK_FILE=%BUILD_DIR%\%~3_QtDeploy.lock"
set "WINDEPLOY_EXE=%~2"
set "QML_DIR=%~4"
set "TARGET_FILE_PATH=%~5"
set "ARCH=x64"

if not exist "%LOCK_FILE%" (
    echo Creating lock file : "%LOCK_FILE%"
    echo. 2> "%LOCK_FILE%"
    echo Deploying Qt libraries...
    if "%WINDEPLOY_EXE%"=="" (
        echo ERROR: windeployqt.exe not found in PATH.
        del "%LOCK_FILE%"
        exit /b 1
    )
    set "QML_DEPLOY=--qmldir "%QML_DIR%"
    if %QML_DIR%=="None" (
        set "QML_DEPLOY="
    )

    echo "%BUILD_DIR%\vcpkg_installed\%ARCH%-windows\tools\Qt6\bin\%WINDEPLOY_EXE%" ^
                  --force-openssl ^
                  %QML_DEPLOY% ^
                  "%TARGET_FILE_PATH%"
    call "%BUILD_DIR%\vcpkg_installed\%ARCH%-windows\tools\Qt6\bin\%WINDEPLOY_EXE%" ^
         --force-openssl ^
         --qmldir "%QML_DIR%" ^
         "%TARGET_FILE_PATH%"
    if $? == False (
        echo Deploy failed with error code %errorlevel%.
        echo "Deleting lock file: %LOCK_FILE%"
        del "%LOCK_FILE%"
        exit /b 1
    )
    echo Deploy completed successfully.
) else (
    echo Lock file already exists: "%LOCK_FILE%"
    echo Deploy already in progress or completed. Exiting without changes.
)
