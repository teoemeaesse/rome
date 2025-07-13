#!/bin/bash
set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

usage() {
    echo -e "Usage: $0 [options]"
    echo "Options:"
    echo "  --release, -f    Build in release mode with optimizations."
    echo "  --tests, -t      Build tests along with the core module."
    echo "  --help, -h       Display this help message."
}

RELEASE=0
TESTS=0
for arg in "$@"; do
    case $arg in
        --release|-f)
            RELEASE=1
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        --tests|-t)
            TESTS=1
            ;;
        *)
            echo -e "${RED}Unknown option: $arg${NC}"
            usage
            exit 1
            ;;
    esac
done

echo -e "${GREEN}Creating build directory for core and entering it...${NC}"
mkdir -p build
cd build || { echo -e "${RED}Failed to enter core/build directory. Exiting...${NC}"; exit 1; }

if [ "$RELEASE" -eq 1 ]; then
    echo -e "${GREEN}Configuring core for release build...${NC}"
    cmake -DCMAKE_BUILD_TYPE=Release ..
else
    echo -e "${GREEN}Configuring core for debug build...${NC}"
    cmake -DCMAKE_BUILD_TYPE=Debug ..
fi

if [ "$TESTS" -eq 1 ]; then
    echo -e "${GREEN}Building core module with tests...${NC}"
    cmake -DBUILD_TESTS=ON ..
fi

echo -e "${GREEN}Building core module...${NC}"
cmake --build .

echo -e "${GREEN}Core build script completed.${NC}"

if [ "$TESTS" -eq 1 ]; then
    echo -e "${GREEN}Running core tests...${NC}"
    make test
    echo -e "${GREEN}Core tests completed.${NC}"
fi