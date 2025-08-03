@echo off
REM Check if Python is installed
where python3 >nul 2>&1
if %errorlevel% equ 0 (
    set pythonCmd=python3
) else (
    where python >nul 2>&1
    if %errorlevel% equ 0 (
        set pythonCmd=python
    ) else (
        echo Python is not installed. Please install Python to use this script.
        exit /b 1
    )
)

REM Check if pip is installed
where pip3 >nul 2>&1
if %errorlevel% equ 0 (
    set pipCmd=pip3
) else (
    where pip >nul 2>&1
    if %errorlevel% equ 0 (
        set pipCmd=pip
    ) else (
        echo Pip is not installed. Please install pip to use this script.
        exit /b 1
    )
)

REM Move working directory to script location
cd /d %~dp0

REM Create virtual environment if it doesn't exist
if not exist ".venv" (
    echo Creating virtual environment...
    %pythonCmd% -m venv .venv
)

REM Activate virtual environment
if exist ".venv\Scripts\activate.bat" (
    call ".venv\Scripts\activate.bat"
) else (
    echo Virtual environment activation script not found.
    exit /b 1
)

REM Install requirements if requirements.txt exists
if exist "tools\requirements.txt" (
    echo Installing requirements...
    %pipCmd% install -r tools\requirements.txt
) else (
    echo requirements.txt not found. Skipping installation.
)
REM Keep the virtual environment active after script ends
cmd /k
