#!/bin/bash
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${GREEN}Fetching submodules...${NC}"
git submodule update --init --recursive

echo -e "${GREEN}Building core dependencies...${NC}"
cd core/lib/SDL

echo -e "${GREEN}Building SDL...${NC}"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DSDL_STATIC=ON && cmake --build build && cmake --install build

echo -e "${GREEN}Done!${NC}"