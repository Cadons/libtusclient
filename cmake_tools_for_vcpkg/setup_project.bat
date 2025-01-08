@echo off
SETLOCAL
cd /d "%~dp0"
@echo off
SETLOCAL
cd /d "%~dp0"

REM Check for skip-submodule argument
SET "SKIP_SUBMODULE=false"
IF "%1"=="skip-submodule" (
    SET "SKIP_SUBMODULE=true"
)

echo Initializing project...
cd ..
REM Clone submodules if skip-submodule argument is not passed
IF "%SKIP_SUBMODULE%"=="false" (
    echo Cloning submodules...
    git submodule update --init --recursive || set "errorlevel=1"
) ELSE (
    echo Skipping submodule initialization.
)

cd cmake_tools_for_vcpkg
REM Create virtual environment
echo Creating virtual environment...
python -m venv venv

REM Activate virtual environment
call venv\Scripts\activate

REM Install requirements
IF EXIST "tools\requirements.txt" (
    echo Installing requirements from requirements.txt...
    pip install -r tools\requirements.txt || set "errorlevel=1"
) ELSE (
    echo No requirements.txt found, skipping installation.
)

REM Run the setup script
echo Running setup.py...
python tools\setup.py || set "errorlevel=1"

IF %errorlevel% NEQ 0 (
    echo Project initialization failed.
    exit /b 1
)

echo Project initialized successfully.
ENDLOCAL
