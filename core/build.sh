#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

usage() {
  echo -e "Usage: $0 [options]"
  echo "Options:"
  echo "  --release, -f           Build in Release mode (default: Debug)"
  echo "  --tests, -t             Configure & build tests (BUILD_TESTS=ON), then run them"
  echo "  --fresh                 Delete build/ before configuring"
  echo "  --install, -i           Install core package after build (Option B)"
  echo "  --prefix <path>         Install prefix (default: ../dist)"
  echo "  --help, -h              Show this help"
}

BUILD_TYPE="Debug"
BUILD_TESTS="OFF"
FRESH=0
DO_INSTALL=0
# core/ is current dir when running this script, so ../dist is repo-root/dist
INSTALL_PREFIX="../dist"

# Parse args (supports --prefix value)
while [ $# -gt 0 ]; do
  case "$1" in
    --release|-f) BUILD_TYPE="Release"; shift ;;
    --tests|-t)   BUILD_TESTS="ON"; shift ;;
    --fresh)      FRESH=1; shift ;;
    --install|-i) DO_INSTALL=1; shift ;;
    --prefix)
      shift
      if [ -z "${1:-}" ]; then
        echo -e "${RED}--prefix requires a path.${NC}"
        exit 1
      fi
      INSTALL_PREFIX="$1"
      shift
      ;;
    --help|-h) usage; exit 0 ;;
    *)
      echo -e "${RED}Unknown option: $1${NC}"
      usage
      exit 1
      ;;
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

if [ $DO_INSTALL -eq 1 ]; then
  echo -e "${GREEN}Installing core to ${INSTALL_PREFIX}...${NC}"
  cmake --install build --prefix "${INSTALL_PREFIX}"
fi

echo -e "${GREEN}Done.${NC}"
