#!/bin/bash

# List of log files to clean
LOG_FILES=("log.txt" "log1.txt" "log2.txt")

# Iterate through each log file and clear its contents
for FILE in "${LOG_FILES[@]}"; do
    if [ -f "$FILE" ]; then
        echo "Cleaning $FILE"
        > "$FILE"
    else
        echo "File $FILE does not exist"
    fi
done

echo "Log files cleaned."