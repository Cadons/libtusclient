@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

REM Check for arguments
SET "SKIP_SUBMODULE=false"
SET "RESET=false"

IF /I "%1"=="skip-submodule" (
    SET "SKIP_SUBMODULE=true"
) ELSE IF /I "%1"=="new" (
    SET "RESET=true"
)

echo Initializing project...

REM Clone submodules if skip-submodule argument is not passed
IF /I "%SKIP_SUBMODULE%"=="false" (
    echo Cloning submodules...
    git submodule update --init --recursive || (
        echo Error: Failed to clone submodules.
        EXIT /B 1
    )
) ELSE (
    echo Skipping submodule initialization.
)

REM Check for Python, Git, pip, Vcpkg, and CMake availability
echo Checking availability of Python, Git, Vcpkg, CMake, and pip...

python --version >nul 2>&1
IF ERRORLEVEL 1 (
    echo Error: Python is not available. Please install Python and try again.
    EXIT /B 1
)

git --version >nul 2>&1
IF ERRORLEVEL 1 (
    echo Error: Git is not available. Please install Git and try again.
    EXIT /B 1
)

vcpkg --version >nul 2>&1
IF ERRORLEVEL 1 (
    echo Error: Vcpkg is not available. Please install Vcpkg and try again.
    EXIT /B 1
)

cmake --version >nul 2>&1
IF ERRORLEVEL 1 (
    echo Error: CMake is not available. Please install CMake and try again.
    EXIT /B 1
)

pip --version >nul 2>&1
IF ERRORLEVEL 1 (
    echo Error: pip is not available. Please ensure pip is installed with Python.
    EXIT /B 1
)

REM Move to .vpm directory and configure project
cd .vpm || (
    echo Error: Failed to change directory to .vpm.
    EXIT /B 1
)

REM Check if virtual environment exists
IF EXIST venv (
    echo Virtual environment already exists.
) ELSE (
    echo Creating virtual environment...
    python -m venv venv
    IF ERRORLEVEL 1 (
        echo Error: Failed to create virtual environment.
        EXIT /B 1
    )
)

REM Activate virtual environment
call venv\Scripts\activate.bat >nul 2>&1
IF ERRORLEVEL 1 (
    echo Error: Failed to activate virtual environment.
    EXIT /B 1
)

REM Install requirements
IF EXIST "requirements.txt" (
    echo Installing requirements from requirements.txt...
    pip install -r requirements.txt
    IF ERRORLEVEL 1 (
        echo Error: Failed to install requirements.
        EXIT /B 1
    )
) ELSE (
    echo No requirements.txt found, skipping installation.
)

REM If RESET is true, ask for confirmation
IF /I "%RESET%"=="true" (
    echo Warning: This will remove all project configurations, including vcpkg dependencies and CMake build files.
    CHOICE /M "Are you sure you want to continue?" /C YN
    IF ERRORLEVEL 2 (
        echo Operation cancelled by user.
        call venv\Scripts\deactivate.bat >nul 2>&1
        EXIT /B 0
    )
    echo Running with --reset argument...
    python src\vpm.py --reset
    IF ERRORLEVEL 1 (
        echo Error: vpm.py encountered an issue with --reset.
        EXIT /B 1
    )
) ELSE (
    echo Running initialization script...
    python src\vpm.py
    IF ERRORLEVEL 1 (
        echo Error: vpm.py encountered an issue.
        EXIT /B 1
    )
)

REM Deactivate virtual environment if the script exists
IF EXIST venv\Scripts\deactivate.bat (
    call venv\Scripts\deactivate.bat >nul 2>&1
    IF ERRORLEVEL 1 (
        echo Warning: Failed to deactivate virtual environment.
    )
)

cd ..

echo Project setup completed successfully.
exit /B 0
