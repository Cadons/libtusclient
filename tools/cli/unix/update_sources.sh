#!/bin/bash

echo "Welcome to Module Sync Script"
echo "----------------------------"

read -p "Enter directory path: " path
#move working dir to root/tools
cd "$(dirname "$0")/../.."
cmd="python dev_tools.py update sync \"$path\""



# Execute the command if option 1 or 2 was selected
if [ "$option_number" != "3" ]; then
    echo -e "\nExecuting: $cmd"
    eval $cmd
fi