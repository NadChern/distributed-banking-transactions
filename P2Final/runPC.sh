#!/bin/bash

# Define colors for output
RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
YELLOW='\033[0;33m'

# Function to run participants
run_participants() {
    echo -e "${YELLOW}Running participants...${NC}"

    # Run participant processes in parallel
    echo -e "${GREEN}Running participant 1${NC}"
    ./participant 2233 acc1.txt log1.txt & # & to run in background
    PID1=$!

    echo -e "${GREEN}Running participant 2${NC}"
    ./participant 2234 acc2.txt log2.txt &
    PID2=$!

    # Wait for all background processes to complete
    wait $PID1 $PID2
}

# Function to run coordinator
run_coordinator() {
    echo -e "${YELLOW}Running coordinator...${NC}"

    # Run the coordinator process
    ./coordinator log.txt 100 localhost 2233 0982838-88 localhost 2234 0933310-04-27.6
    PID3=$!

    # Wait for the coordinator process to complete
    wait $PID3
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error running coordinator. ${NC}"
        exit 1
    fi

    # Wait for the coordinator process to complete
    wait $PID3
}

# Main script logic to handle arguments
# Check if the argument is "participants" or "coordinator"
if [[ "$1" == "participants" ]]; then
    run_participants
elif [[ "$1" == "coordinator" ]]; then
    run_coordinator
# Display usage if the argument is invalid
else
    echo -e "${RED}Usage: $0 {participants|coordinator}${NC}"
    exit 1
fi

echo -e "${GREEN}All processes completed successfully${NC}"