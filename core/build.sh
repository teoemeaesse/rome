#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

usage() {
  echo -e "Usage: $0 [options]"
  echo "Options:"
  echo "  --release, -f    Build in Release mode (default: Debug)"
  echo "  --tests, -t      Configure & build tests (BUILD_TESTS=ON), then run them"
  echo "  --fresh          Delete build/ before configuring"
  echo "  --help, -h       Show this help"
}

BUILD_TYPE="Debug"
BUILD_TESTS="OFF"
FRESH=0

for arg in "$@"; do
  case "$arg" in
    --release|-f) BUILD_TYPE="Release" ;;
    --tests|-t)   BUILD_TESTS="ON" ;;
    --fresh)      FRESH=1 ;;
    --help|-h)    usage; exit 0 ;;
    *) echo -e "${RED}Unknown option: $arg${NC}"; usage; exit 1 ;;
  esac
done

GENERATOR_ARGS=()
if command -v ninja >/dev/null 2>&1; then
  GENERATOR_ARGS=(-G Ninja)
  echo -e "${GREEN}Using Ninja generator.${NC}"
else
  echo -e "${YELLOW}Ninja not found; using CMake's default generator.${NC}"
fi

if [ $FRESH -eq 1 ]; then
  echo -e "${GREEN}Removing build/ (fresh configure)...${NC}"
  rm -rf build
fi

echo -e "${GREEN}Configuring (type=${BUILD_TYPE}, tests=${BUILD_TESTS})...${NC}"
cmake -S . -B build "${GENERATOR_ARGS[@]}" \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DBUILD_TESTS="${BUILD_TESTS}" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

if [ -f build/compile_commands.json ]; then
  echo -e "${GREEN}Linking compile_commands.json to project root...${NC}"
  ln -sfn build/compile_commands.json compile_commands.json
else
  echo -e "${RED}compile_commands.json not found in build/. Did configure fail?${NC}"
  exit 1
fi

if command -v nproc >/dev/null 2>&1; then
  JOBS=$(nproc)
elif [[ "${OSTYPE:-}" == "darwin"* ]]; then
  JOBS=$(sysctl -n hw.ncpu)
else
  JOBS=4
fi

echo -e "${GREEN}Building with ${JOBS} parallel jobs...${NC}"
cmake --build build -- -j"${JOBS}"

if [ "${BUILD_TESTS}" = "ON" ]; then
  echo -e "${GREEN}Running tests...${NC}"
  ctest --test-dir build --output-on-failure
fi

echo -e "${GREEN}Done.${NC}"
