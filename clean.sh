#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}Cleaning build directories...${NC}"

# Delete the build directory in core
if [ -d "core/build" ]; then
    echo -e "${GREEN}Removing core/build...${NC}"
    rm -rf core/build
    rm -rf core/bin
else
    echo -e "${RED}core/build does not exist. Skipping...${NC}"
fi

# Delete the build directory in engine
if [ -d "engine/build" ]; then
    echo -e "${GREEN}Removing engine/build...${NC}"
    rm -rf engine/build
    rm -rf engine/bin
else
    echo -e "${RED}engine/build does not exist. Skipping...${NC}"
fi

# Delete the build directory in sandbox
if [ -d "sandbox/build" ]; then
    echo -e "${GREEN}Removing sandbox/build...${NC}"
    rm -rf sandbox/build
    rm -rf sandbox/bin
else
    echo -e "${RED}sandbox/build does not exist. Skipping...${NC}"
fi

# Optionally, remove binaries in core/bin and engine/bin if you want a full clean
# Uncomment the following lines if needed
# echo -e "${GREEN}Cleaning bin directories...${NC}"
# rm -rf core/bin/*
# rm -rf engine/bin/*

echo -e "${GREEN}Clean script completed.${NC}"