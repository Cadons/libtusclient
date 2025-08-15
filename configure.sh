#!/bin/bash
#check if python is installed or python3
if command -v python3 &>/dev/null; then
    PYTHON_CMD="python3"
elif command -v python &>/dev/null; then
    PYTHON_CMD="python"
else
    echo "Python is not installed. Please install Python to use this script."
    exit 1
fi
#pip or pip3
if command -v pip3 &>/dev/null; then
    PIP_CMD="pip3"
elif command -v pip &>/dev/null; then
    PIP_CMD="pip"
else
    echo "Pip is not installed. Please install pip to use this script."
    exit 1
fi

#move working dir to root/
cd "$(dirname "$0")"

#create virtual environment
if [ ! -d "venv" ]; then
    echo "Creating virtual environment..."
    $PYTHON_CMD -m venv .venv
fi
#activate virtual environment
if [ -f ".venv/bin/activate" ]; then
    source .venv/bin/activate
elif [ -f ".venv/Scripts/activate" ]; then
    source .venv/Scripts/activate
else
    echo "Virtual environment activation script not found."
    exit 1
fi
cd tools
#install requirements
if [ -f "requirements.txt" ]; then
    echo "Installing requirements..."
    $PIP_CMD install -r requirements.txt
else
    echo "requirements.txt not found. Skipping installation."
fi
cd ..
# Keep the virtual environment active after script ends
$SHELL
