#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

BUILD_TYPE="standard"
BUILD_FLAG_FILE=".last_build"

usage() {
    echo -e "Usage: $0 [options]"
    echo "Options:"
    echo "  --release, -f    Build in release mode with optimizations."
    echo "  --tests, -t      Build core tests."
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
            BUILD_TYPE="tests"
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

if [ -f "$BUILD_FLAG_FILE" ]; then
    LAST_BUILD_TYPE=$(cat $BUILD_FLAG_FILE)
    if [ "$LAST_BUILD_TYPE" != "$BUILD_TYPE" ]; then
        echo -e "${YELLOW}Build type has changed (last: $LAST_BUILD_TYPE, current: $BUILD_TYPE). Cleaning up...${NC}"
        ./clean.sh
    fi
fi

echo "$BUILD_TYPE" > "$BUILD_FLAG_FILE"

echo -e "${GREEN}Building core module...${NC}"
(cd core && ./build.sh $RELEASE_FLAG $TESTS_FLAG)

if [ "$TESTS" -eq 0 ]; then
    echo -e "${GREEN}Building engine module...${NC}"
    (cd engine && ./build.sh $RELEASE_FLAG)
    echo -e "${GREEN}Building sandbox module...${NC}"
    (cd sandbox && ./build.sh $RELEASE_FLAG)
else
    echo -e "${YELLOW}Skipping engine and sandbox modules because tests are enabled.${NC}"
fi

echo -e "${GREEN}Root build script completed.${NC}"

cat ./core/build/Testing/Temporary/LastTest.log  