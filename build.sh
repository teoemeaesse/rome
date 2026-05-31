#!/bin/bash
set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

BUILD_TYPE="standard"
BUILD_FLAG_FILE=".last_build"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
CORE_PREFIX="${BUILD_DIR}/core-package"
DIST_DIR="${PROJECT_ROOT}/dist"

usage() {
    echo -e "Usage: $0 [options]"
    echo "Options:"
    echo "  --release, -f    Build in release mode with optimizations."
    echo "  --tests, -t      Build core tests."
    echo "  --dist, -d       Create a distributable package in ./dist."
    echo "  --help, -h       Display this help message."
}

RELEASE=0
TESTS=0
DIST=0

cd "$PROJECT_ROOT"

for arg in "$@"; do
    case "$arg" in
        --release|-f)
            RELEASE=1
            ;;
        --tests|-t)
            TESTS=1
            BUILD_TYPE="tests"
            ;;
        --dist|-d)
            DIST=1
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
    LAST_BUILD_TYPE=$(cat "$BUILD_FLAG_FILE")
    if [ "$LAST_BUILD_TYPE" != "$BUILD_TYPE" ]; then
        echo -e "${YELLOW}Build type has changed (last: $LAST_BUILD_TYPE, current: $BUILD_TYPE). Cleaning up...${NC}"
        ./clean.sh
    fi
fi

echo "$BUILD_TYPE" > "$BUILD_FLAG_FILE"

package_dist() {
    echo -e "${GREEN}Creating distribution package at ${DIST_DIR}...${NC}"
    rm -rf "$DIST_DIR"
    mkdir -p "$DIST_DIR"

    cp -R "${CORE_PREFIX}/." "$DIST_DIR/"

    mkdir -p "${DIST_DIR}/bin" "${DIST_DIR}/lib/plugins" "${DIST_DIR}/plugins"

    if [ -f "${PROJECT_ROOT}/sandbox/bin/sandbox" ]; then
        cp "${PROJECT_ROOT}/sandbox/bin/sandbox" "${DIST_DIR}/bin/"
        if command -v install_name_tool >/dev/null 2>&1; then
            install_name_tool -delete_rpath "${CORE_PREFIX}/lib" "${DIST_DIR}/bin/sandbox" 2>/dev/null || true
            install_name_tool -delete_rpath "${DIST_DIR}/lib" "${DIST_DIR}/bin/sandbox" 2>/dev/null || true
            install_name_tool -add_rpath "@loader_path/../lib" "${DIST_DIR}/bin/sandbox" 2>/dev/null || true
        fi
    fi

    shopt -s nullglob
    for plugin_dir in "${PROJECT_ROOT}"/engine/plugins/*; do
        if [ ! -d "$plugin_dir" ]; then
            continue
        fi

        plugin_name="$(basename "$plugin_dir")"
        plugin_package_dir="${DIST_DIR}/plugins/${plugin_name}"

        if [ -d "${plugin_dir}/include" ]; then
            cp -R "${plugin_dir}/include/." "${DIST_DIR}/include/"
            mkdir -p "${plugin_package_dir}"
            cp -R "${plugin_dir}/include" "${plugin_package_dir}/"
        fi

        if [ -f "${plugin_dir}/plugin.json" ]; then
            mkdir -p "${plugin_package_dir}"
            cp "${plugin_dir}/plugin.json" "${plugin_package_dir}/"
        fi

        for plugin_lib in "${plugin_dir}"/bin/*.dylib "${plugin_dir}"/bin/*.so; do
            if [ ! -f "$plugin_lib" ]; then
                continue
            fi

            packaged_plugin_lib="${DIST_DIR}/lib/plugins/$(basename "$plugin_lib")"
            cp "$plugin_lib" "$packaged_plugin_lib"

            if command -v install_name_tool >/dev/null 2>&1; then
                install_name_tool -delete_rpath "${CORE_PREFIX}/lib" "$packaged_plugin_lib" 2>/dev/null || true
                install_name_tool -delete_rpath "${DIST_DIR}/lib" "$packaged_plugin_lib" 2>/dev/null || true
                install_name_tool -add_rpath "@loader_path/.." "$packaged_plugin_lib" 2>/dev/null || true
            fi
        done
    done
    shopt -u nullglob

    find "$DIST_DIR" -name ".DS_Store" -delete
}

echo -e "${GREEN}Building core module...${NC}"
if [ "$TESTS" -eq 0 ]; then
    rm -rf "$CORE_PREFIX"
    mkdir -p "$CORE_PREFIX"
    (cd core && ./build.sh $RELEASE_FLAG --install --prefix "$CORE_PREFIX")
else
    (cd core && ./build.sh $RELEASE_FLAG $TESTS_FLAG)
fi

echo -e "${GREEN}Core build completed.${NC}"

if [ "$TESTS" -eq 0 ]; then
    echo -e "${GREEN}Building engine module (CORE_PREFIX=${CORE_PREFIX})...${NC}"
    (cd engine && ./build.sh $RELEASE_FLAG --core-prefix "$CORE_PREFIX")

    echo -e "${GREEN}Building sandbox module...${NC}"
    (cd sandbox && ./build.sh $RELEASE_FLAG --core-prefix "$CORE_PREFIX")

    if [ "$DIST" -eq 1 ]; then
        package_dist
    fi
else
    echo -e "${YELLOW}Skipping engine and sandbox modules because tests are enabled.${NC}"
    if [ "$DIST" -eq 1 ]; then
        echo -e "${YELLOW}Skipping dist package because tests are enabled.${NC}"
    fi
fi

echo -e "${GREEN}Root build script completed.${NC}"

if [ "$TESTS" -eq 1 ]; then
    echo -e "${GREEN}Test results:${NC}"
    if [ -f "./core/build/Testing/Temporary/LastTest.log" ]; then
        cat ./core/build/Testing/Temporary/LastTest.log
    else
        echo -e "${YELLOW}No LastTest.log found. Tests may not have run (ctest).${NC}"
    fi
fi
