#!/bin/bash

echo "Welcome to Quick Operations Script"
echo "----------------------------------"

# Go to project root (2 levels up from script dir)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.." || exit 1
PYTHON_CMD="$(command -v python3 || command -v python)"
if ! PYTHON_CMD="$(command -v python3 || command -v python)"; then
    echo "Error: Python not found in PATH"
    exit 1
fi
# Select operation
echo "Select an operation:"
echo "1) Create new source/template"
echo "2) Update existing sources"

while true; do
    read -rp "Choose operation (1-2): " operation_number
    case "$operation_number" in
        1) operation="create"; break ;;
        2) operation="update"; break ;;
        *) echo "Invalid selection. Please choose 1 or 2." ;;
    esac
done

# ---- CREATE ----
if [[ "$operation" == "create" ]]; then
    echo
    echo "Available template types:"
    echo "1) class - Standard C++ Class"
    echo "2) gtest - Google Test"
    echo "3) gtest_fixture - Google Test with Fixture"
    echo "4) gtest_parametrized - Parametrized Google Test"
    echo "5) qrc - Qt Resource File"

    while true; do
        read -rp "Select template type (1-5): " template_number
        case "$template_number" in
            1) template_type="class"; break ;;
            2) template_type="gtest"; break ;;
            3) template_type="gtest_fixture"; break ;;
            4) template_type="gtest_parametrized"; break ;;
            5) template_type="qrc"; break ;;
            *) echo "Invalid selection. Please choose a number between 1 and 5." ;;
        esac
    done

    # Always ask for name
    read -rp "Enter name: " name

    # Start command
    cmd=("$PYTHON_CMD" dev_tools.py source create --type "$template_type" --name "$name")

    # Add optional args only for non-qrc
    if [[ "$template_type" != "qrc" ]]; then
        read -rp "Enter namespace (press Enter for none): " namespace


        [[ -n "$namespace" ]] && cmd+=("--namespace" "$namespace")

    fi
    read -rp "Enter output directory (press Enter for 'src'): " output_dir

     # Default output directory
     if [[ -z "$output_dir" ]]; then
         output_dir="$SCRIPT_DIR/../../../src"
     fi
           cmd+=("--output-dir" "$output_dir")
# ---- UPDATE ----
elif [[ "$operation" == "update" ]]; then
    read -rp "Enter directory path to update sources: " update_path
    cmd=("$PYTHON_CMD" dev_tools.py update sync "$update_path")
fi

# ---- EXECUTE ----
echo
echo "Executing: ${cmd[*]}"
"${cmd[@]}"
