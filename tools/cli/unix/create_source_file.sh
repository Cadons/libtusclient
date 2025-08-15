#!/bin/bash

echo "Welcome to Quick Operations Script"
echo "--------------------------------"
#move working dir to root/tools
cd "$(dirname "$0")/../.."
# Display template options
echo "Available template types:"
echo "1) class - Standard C++ Class"
echo "2) gtest - Google Test"
echo "3) gtest_fixture - Google Test with Fixture"
echo "4) gtest_parametrized - Parametrized Google Test"
echo "5) qrc - Qt Resource File"

# Get template selection
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

# Request remaining inputs
read -p "Enter name for the new file/class: " name
read -p "Enter namespace (press Enter for none): " namespace
read -p "Enter output directory (press Enter for 'src'): " output_dir

# Set default output directory if empty
if [ -z "$output_dir" ]; then
    output_dir="src"
fi

# Construct the command
cmd="python dev_tools.py source create"
cmd="$cmd --type '$template_type'"
cmd="$cmd --name '$name'"

if [ ! -z "$namespace" ]; then
    cmd="$cmd --namespace '$namespace'"
fi

cmd="$cmd --output-dir '$output_dir'"

# Execute the command
echo -e "\nExecuting: $cmd"
eval $cmd