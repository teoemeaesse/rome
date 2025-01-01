#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

usage() {
    echo -e "Usage: $0 [options]"
    echo "Options:"
    echo "  --release, -f    Build in release mode with optimizations."
    echo "  --tests, -t      Build with tests enabled."
    echo "  --help, -h       Display this help message."
}

RUN=0
RELEASE=0
TESTS=0
for arg in "$@"; do
    case $arg in
        --release|-f)
            RELEASE=1
            ;;
        --tests|-t)
            TESTS=1
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

RELEASE_FLAG=""
if [ "$RELEASE" -eq 1 ]; then
    RELEASE_FLAG="--release"
fi

TESTS_FLAG=""
if [ "$TESTS" -eq 1 ]; then
    TESTS_FLAG="--tests"
fi

echo -e "${GREEN}Building core module...${NC}"
(cd core && ./build.sh $RELEASE_FLAG $TESTS_FLAG)
echo -e "${GREEN}Building engine module...${NC}"
(cd engine && ./build.sh $RELEASE_FLAG)
echo -e "${GREEN}Building sandbox module...${NC}"
(cd sandbox && ./build.sh $RELEASE_FLAG)

echo -e "${GREEN}Root build script completed.${NC}"