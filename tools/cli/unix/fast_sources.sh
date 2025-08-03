#!/bin/bash

echo "Welcome to Quick Operations Script"
echo "--------------------------------"

# Change working directory to project root (2 levels up from current script)
cd "$(dirname "$0")/../.."

# Display operation options
echo "Select an operation:"
echo "1) Create new source/template"
echo "2) Update existing sources"

# Prompt user for operation selection
while true; do
    read -p "Choose operation (1-2): " operation_number
    case $operation_number in
        1) operation="create" ; break ;;
        2) operation="update" ; break ;;
        *) echo "Invalid selection. Please choose 1 or 2." ;;
    esac
done

# If operation is "create", prompt for template details
if [ "$operation" == "create" ]; then
    echo
    echo "Available template types:"
    echo "1) class - Standard C++ Class"
    echo "2) gtest - Google Test"
    echo "3) gtest_fixture - Google Test with Fixture"
    echo "4) gtest_parametrized - Parametrized Google Test"
    echo "5) qrc - Qt Resource File"

    # Prompt user for template type selection
    while true; do
        read -p "Select template type (1-5): " template_number
        case $template_number in
            1) template_type="class" ; break ;;
            2) template_type="gtest" ; break ;;
            3) template_type="gtest_fixture" ; break ;;
            4) template_type="gtest_parametrized" ; break ;;
            5) template_type="qrc" ; break ;;
            *) echo "Invalid selection. Please choose a number between 1 and 5." ;;
        esac
    done

    # Prompt for additional parameters
    read -p "Enter name for the new file/class: " name
    read -p "Enter namespace (press Enter for none): " namespace
    read -p "Enter output directory (press Enter for 'src'): " output_dir

    # Set default output directory if empty
    if [ -z "$output_dir" ]; then
        output_dir="$(dirname "$0")../../../src"
    fi

    # Build the create command
    cmd="python dev_tools.py source create"
    cmd="$cmd --type '$template_type'"
    cmd="$cmd --name '$name'"

    if [ -n "$namespace" ]; then
        cmd="$cmd --namespace '$namespace'"
    fi

    cmd="$cmd --output-dir '$output_dir'"

# If operation is "update", prompt for directory path
elif [ "$operation" == "update" ]; then
    read -p "Enter directory path to update sources: " path
    cmd="python dev_tools.py update sync \"$path\""
fi

# Execute the constructed command
echo -e "\nExecuting: $cmd"
eval $cmd
cd ..
