#!/bin/bash
set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

usage() {
    echo -e "Usage: $0 [options]"
    echo "Options:"
    echo "  --release, -f              Build in release mode with optimizations."
    echo "  --core-prefix <path>       Path to core install prefix (dist folder). Default: ../../dist"
    echo "  --help, -h                 Display this help message."
}

RELEASE=0
CORE_PREFIX="../../dist"

while [ $# -gt 0 ]; do
    case "$1" in
        --release|-f)
            RELEASE=1
            shift
            ;;
        --core-prefix)
            shift
            if [ -z "$1" ]; then
                echo -e "${RED}--core-prefix requires a path.${NC}"
                exit 1
            fi
            CORE_PREFIX="$1"
            shift
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            usage
            exit 1
            ;;
    esac
done

echo -e "${GREEN}Creating build directory for engine and entering it...${NC}"
mkdir -p build
cd build || { echo -e "${RED}Failed to enter engine/build directory. Exiting...${NC}"; exit 1; }

if [ "$RELEASE" -eq 1 ]; then
    echo -e "${GREEN}Configuring engine for release build...${NC}"
    cmake -DCMAKE_BUILD_TYPE=Release -DCORE_PREFIX="$CORE_PREFIX" ..
else
    echo -e "${GREEN}Configuring engine for debug build...${NC}"
    cmake -DCMAKE_BUILD_TYPE=Debug -DCORE_PREFIX="$CORE_PREFIX" ..
fi

echo -e "${GREEN}Building engine module...${NC}"
cmake --build .

echo -e "${GREEN}Engine build script completed.${NC}"
