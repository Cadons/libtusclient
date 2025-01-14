#!/bin/bash
set -euo pipefail

# Check for arguments
SKIP_SUBMODULE=false
RESET=false

if [[ "${1:-}" == "skip-submodule" ]]; then
    SKIP_SUBMODULE=true
elif [[ "${1:-}" == "new" ]]; then
    RESET=true
fi

echo "Initializing project..."

# Clone submodules if skip-submodule argument is not passed
if [[ "$SKIP_SUBMODULE" == false ]]; then
    echo "Cloning submodules..."
    git submodule update --init --recursive || {
        echo "Error: Failed to clone submodules."
        exit 1
    }
else
    echo "Skipping submodule initialization."
fi

# Check for Python, Git, Vcpkg, CMake, and pip availability
echo "Checking availability of Python, Git, Vcpkg, CMake, and pip..."

for cmd in python3 git vcpkg cmake pip; do
    if ! command -v "$cmd" &> /dev/null; then
        echo "Error: $cmd is not available. Please install $cmd and try again."
        exit 1
    fi
done

# Move to .vpm directory and configure project
cd .vpm || {
    echo "Error: Failed to change directory to .vpm."
    exit 1
}

# Check if virtual environment exists
if [[ -d "venv" ]]; then
    echo "Virtual environment already exists."
else
    echo "Creating virtual environment..."
    python3 -m venv venv || {
        echo "Error: Failed to create virtual environment."
        exit 1
    }
fi

# Activate virtual environment
source venv/bin/activate

# Install requirements
if [[ -f "requirements.txt" ]]; then
    echo "Installing requirements from requirements.txt..."
    pip install -r requirements.txt || {
        echo "Error: Failed to install requirements."
        exit 1
    }
else
    echo "No requirements.txt found, skipping installation."
fi

# Prompt confirmation if --reset argument is passed
if [[ "$RESET" == true ]]; then
    echo "Warning: This will remove all project configurations, including vcpkg dependencies and CMake build files."
    read -rp "Are you sure you want to continue? [y/N]: " confirm
    if [[ "$confirm" != "y" && "$confirm" != "Y" ]]; then
        echo "Operation cancelled by user."
        deactivate
        exit 0
    fi
    
    echo "Running with --reset argument..."
    python3 src/vpm.py --reset || {
        echo "Error: vpm.py encountered an issue with --reset."
        exit 1
    }
else
    echo "Running initialization script..."
    python3 src/vpm.py || {
        echo "Error: vpm.py encountered an issue."
        exit 1
    }
fi

# Deactivate virtual environment
deactivate

cd ..

echo "Project setup completed successfully."

exit 0
