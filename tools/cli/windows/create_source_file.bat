@echo off
cd /d "%~dp0\..\.."
echo Welcome to Quick Operations Script
echo --------------------------------

:: Display template options
echo Available template types:
echo 1) class - Standard C++ Class
echo 2) gtest - Google Test
echo 3) gtest_fixture - Google Test with Fixture
echo 4) gtest_parametrized - Parametrized Google Test
echo 5) qrc - Qt Resource File

:: Get template selection
:template_selection
set /p template_number="Select template type (1-5): "

:: Set template type based on selection
if "%template_number%"=="1" (
    set template_type=class
) else if "%template_number%"=="2" (
    set template_type=gtest
) else if "%template_number%"=="3" (
    set template_type=gtest_fixture
) else if "%template_number%"=="4" (
    set template_type=gtest_parametrized
) else if "%template_number%"=="5" (
    set template_type=qrc
) else (
    echo Invalid selection. Please choose a number between 1 and 5.
    goto template_selection
)

:: Request remaining inputs
set /p name="Enter name for the new file/class: "
set /p namespace="Enter namespace (press Enter for none): "
set /p output_dir="Enter output directory (press Enter for 'src'): "

:: Set default output directory if empty
if "%output_dir%"=="" set output_dir=src

:: Construct the command
set cmd=python ../../dev_tools.py source create
set cmd=%cmd% --type "%template_type%"
set cmd=%cmd% --name "%name%"

if not "%namespace%"=="" (
    set cmd=%cmd% --namespace "%namespace%"
)

set cmd=%cmd% --output-dir "%output_dir%"

:: Execute the command
echo.
echo Executing: %cmd%
%cmd%

pause