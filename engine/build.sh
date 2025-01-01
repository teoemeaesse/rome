#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

usage() {
    echo -e "Usage: $0 [options]"
    echo "Options:"
    echo "  --release, -f    Build in release mode with optimizations."
    echo "  --help, -h       Display this help message."
}

RELEASE=0
for arg in "$@"; do
    case $arg in
        --release|-f)
            RELEASE=1
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $arg${NC}"
            usage
            exit 1
            ;;
    esac
done

echo -e "${GREEN}Creating build directory for engine and entering it...${NC}"
mkdir -p build
cd build || { echo -e "${RED}Failed to enter engine/build directory. Exiting...${NC}"; exit 1; }

CORE_LIB_PATH="../../core/bin"

if [ "$RELEASE" -eq 1 ]; then
    echo -e "${GREEN}Configuring engine for release build...${NC}"
    cmake -DCMAKE_BUILD_TYPE=Release -DCORE_LIB_PATH="$CORE_LIB_PATH" ..
else
    echo -e "${GREEN}Configuring engine for debug build...${NC}"
    cmake -DCMAKE_BUILD_TYPE=Debug -DCORE_LIB_PATH="$CORE_LIB_PATH" ..
fi

echo -e "${GREEN}Building engine module...${NC}"
cmake --build .

echo -e "${GREEN}Engine build script completed.${NC}"