@echo off
cd /d "%~dp0\..\.."
echo Welcome to Module Sync Script
echo ----------------------------

:menu

set /p path="Enter directory path: "
set cmd=python ../../dev_tools.py update sync "%path%"
goto execute


:execute
echo.
echo Executing: %cmd%
%cmd%

pause