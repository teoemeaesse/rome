#!/bin/bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

usage() {
    echo "Usage: $0 --core <path-to-core> [--debug|--release]"
    echo
    echo "Examples:"
    echo "  $0 --core ../../core"
    echo "  $0 -c /Users/me/rome/core --release"
}

fail() {
    echo -e "${RED}$1${NC}" >&2
    exit 1
}

require_command() {
    local command_name="$1"

    if ! command -v "$command_name" >/dev/null 2>&1; then
        fail "Required command not found: ${command_name}"
    fi
}

require_python() {
    if ! command -v python3 >/dev/null 2>&1; then
        fail "Python 3 is required to read plugin metadata. Install python3 and run this script again."
    fi
}

resolve_dir() {
    local path="$1"

    if [ ! -d "$path" ]; then
        return 1
    fi

    cd "$path" && pwd
}

json_field() {
    local expr="$1"

    python3 - "$SCRIPT_DIR/plugin.json" "$expr" <<'PY'
import json
import sys

json_path = sys.argv[1]
expr = sys.argv[2]

with open(json_path, "r", encoding="utf-8") as f:
    data = json.load(f)

cur = data
for part in expr.split("."):
    cur = cur[part]

print(cur)
PY
}

write_json_digest() {
    local digest="$1"

    python3 - "$SCRIPT_DIR/plugin.json" "$digest" <<'PY'
import json
import sys

json_path = sys.argv[1]
digest = sys.argv[2]

with open(json_path, "r", encoding="utf-8") as f:
    data = json.load(f)

data["digest"] = digest

with open(json_path, "w", encoding="utf-8") as f:
    json.dump(data, f, indent=2)
    f.write("\n")
PY
}

generate_digest() {
    local digest_build_dir

    mkdir -p "$BUILD_DIR"
    digest_build_dir="$(mktemp -d "${BUILD_DIR}/plugin_digest.XXXXXX")"

    (
        trap 'rm -rf "$digest_build_dir"' EXIT

        cat > "${digest_build_dir}/generate_digest.cpp" <<'DIGEST_CPP'
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "rm/crypto/md5.hpp"

namespace fs = std::filesystem;

struct DigestEntry {
    char type;
    std::string relativePath;
    std::string contents;
};

static void append_record(std::string& payload, const DigestEntry& entry) {
    payload += entry.type;
    payload += ':';
    payload += std::to_string(entry.relativePath.size());
    payload += ':';
    payload += entry.relativePath;
    payload += ':';
    payload += std::to_string(entry.contents.size());
    payload += ':';
    payload += entry.contents;
    payload += '\n';
}

static std::string read_file(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to read " + path.string());
    }

    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

static void collect_tree(const fs::path& pluginRoot, const fs::path& root, std::vector<DigestEntry>& entries) {
    if (!fs::exists(root)) {
        throw std::runtime_error("Digest root does not exist: " + root.string());
    }

    entries.push_back({'d', fs::relative(root, pluginRoot).generic_string(), {}});

    fs::recursive_directory_iterator it(root);
    const fs::recursive_directory_iterator end;

    for (; it != end; ++it) {
        const fs::path path = it->path();
        const fs::file_status status = fs::symlink_status(path);

        if (fs::is_symlink(status)) {
            throw std::runtime_error("Digest does not support symlinks under include/ or src/: " + path.string());
        }

        DigestEntry entry{};
        entry.relativePath = fs::relative(path, pluginRoot).generic_string();

        if (fs::is_directory(status)) {
            entry.type = 'd';
        } else if (fs::is_regular_file(status)) {
            entry.type = 'f';
            entry.contents = read_file(path);
        } else {
            throw std::runtime_error("Digest only supports regular files and directories: " + path.string());
        }

        entries.push_back(std::move(entry));
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: generate_digest <plugin-root>\n";
        return 1;
    }

    try {
        const fs::path pluginRoot = fs::weakly_canonical(argv[1]);
        std::vector<DigestEntry> entries;

        collect_tree(pluginRoot, pluginRoot / "include", entries);
        collect_tree(pluginRoot, pluginRoot / "src", entries);

        std::sort(entries.begin(), entries.end(), [](const DigestEntry& lhs, const DigestEntry& rhs) {
            if (lhs.relativePath == rhs.relativePath) {
                return lhs.type < rhs.type;
            }
            return lhs.relativePath < rhs.relativePath;
        });

        std::string payload;
        for (const DigestEntry& entry : entries) {
            append_record(payload, entry);
        }

        std::cout << static_cast<const std::string&>(rome::core::MD5(payload));
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }

    return 0;
}
DIGEST_CPP

        cat > "${digest_build_dir}/CMakeLists.txt" <<DIGEST_CMAKE
cmake_minimum_required(VERSION 3.14)
project(generate_digest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(core CONFIG REQUIRED PATHS "${CORE_PREFIX}" NO_DEFAULT_PATH)

add_executable(generate_digest "${digest_build_dir}/generate_digest.cpp")
target_link_libraries(generate_digest PRIVATE core::core)
set_target_properties(
  generate_digest
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${digest_build_dir}/bin"
             BUILD_RPATH "${CORE_PREFIX}/lib")
DIGEST_CMAKE

        cmake -S "$digest_build_dir" -B "${digest_build_dir}/build" -DCMAKE_BUILD_TYPE=Release >/dev/null
        cmake --build "${digest_build_dir}/build" >/dev/null

        "${digest_build_dir}/bin/generate_digest" "$SCRIPT_DIR"
    )
}

update_digest() {
    local digest

    digest="$(generate_digest)"
    write_json_digest "$digest"
}

require_python
require_command cmake
require_command find
require_command mktemp
require_command uname

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_TYPE="Debug"
CORE_DIR=""

while [ "$#" -gt 0 ]; do
    case "$1" in
        -c|--core)
            option="$1"
            shift
            [ "$#" -gt 0 ] || fail "${option} requires a path."
            CORE_DIR="$1"
            ;;
        --debug)
            BUILD_TYPE="Debug"
            ;;
        --release)
            BUILD_TYPE="Release"
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            fail "Unknown option: $1"
            ;;
    esac
    shift
done

[ -n "$CORE_DIR" ] || {
    usage
    exit 1
}

CORE_DIR="$(resolve_dir "$CORE_DIR")" || fail "Core directory does not exist: ${CORE_DIR}"

if [ ! -f "${CORE_DIR}/CMakeLists.txt" ] || [ ! -d "${CORE_DIR}/include/rm" ]; then
    fail "Expected --core/-c to point at the core source directory."
fi

CORE_PREFIX="$(cd "${CORE_DIR}/.." && pwd)/dist"
CORE_CONFIG="${CORE_PREFIX}/lib/cmake/core/coreConfig.cmake"

if [ ! -f "$CORE_CONFIG" ]; then
    fail "Core package not found at ${CORE_CONFIG}. Build/install core first with core/build.sh."
fi

BUILD_DIR="${SCRIPT_DIR}/build"
OUTPUT_DIR="${SCRIPT_DIR}/bin"

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR" "$OUTPUT_DIR"

update_digest

cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCORE_PREFIX="$CORE_PREFIX"
cmake --build "$BUILD_DIR"

echo -e "${GREEN}Built plugin:${NC} ${OUTPUT_DIR}"
