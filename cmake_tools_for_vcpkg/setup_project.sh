#!/bin/bash
set -e

# Check for skip-submodule argument
SKIP_SUBMODULE=false
if [ "$1" == "skip-submodule" ]; then
    SKIP_SUBMODULE=true
fi

echo "Initializing project..."
cd "$(dirname "$0")/.."

# Clone submodules if skip-submodule argument is not passed
if [ "$SKIP_SUBMODULE" = false ]; then
    echo "Cloning submodules..."
    git submodule update --init --recursive || errorlevel=1
else
    echo "Skipping submodule initialization."
fi
cd cmake_tools_for_vcpkg
# Create and activate virtual environment
echo "Creating virtual environment..."
python3 -m venv venv

# Activate virtual environment
echo "Activating virtual environment..."
source venv/bin/activate

# Install requirements
if [ -f "tools/requirements.txt" ]; then
    echo "Installing requirements from requirements.txt..."
    pip install -r tools/requirements.txt
else
    echo "No requirements.txt found, skipping installation."
fi

# Run the setup script
echo "Running setup.py..."
python3 tools/setup.py

# Print completion message
echo "Project initialized successfully."
