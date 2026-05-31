#!/bin/bash
set -euo pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
NC='\033[0m'

usage() {
    echo "Usage: $0 <PluginName> [output-dir]"
    echo
    echo "If output-dir is omitted, the plugin is created under engine/plugins."
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
        fail "Python 3 is required to write plugin metadata. Install python3 and run this script again."
    fi
}

resolve_output_parent() {
    local repo_root="$1"
    local output_dir="$2"

    python3 - "$repo_root" "$output_dir" <<'PY'
import os
import sys

repo_root = os.path.realpath(sys.argv[1])
output_dir = sys.argv[2]

if os.path.isabs(output_dir):
    resolved = os.path.normpath(output_dir)
else:
    resolved = os.path.normpath(os.path.join(repo_root, output_dir))

print(resolved)
PY
}

confirm_create() {
    local answer

    echo -e "${YELLOW}Create plugin template?${NC}"
    echo "  Plugin: ${PLUGIN_NAME}"
    echo "  Directory will be created: ${PLUGIN_ROOT}"
    echo "  Version: ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
    echo "  Author: ${AUTHOR}"
    echo "  Date: ${DATE}"
    echo
    read -r -p "Proceed? y/n " answer || fail "Aborted."

    case "$answer" in
        y|Y|yes|Yes|YES)
            ;;
        *)
            fail "Aborted."
            ;;
    esac
}

generate_uuid() {
    local core_dir="$1"
    local core_prefix
    local core_config
    local build_dir

    if [ ! -f "${core_dir}/CMakeLists.txt" ] || [ ! -d "${core_dir}/include/rm" ]; then
        fail "Expected core source directory at ${core_dir}."
    fi

    core_prefix="$(cd "${core_dir}/.." && pwd)/dist"
    core_config="${core_prefix}/lib/cmake/core/coreConfig.cmake"

    if [ ! -f "$core_config" ]; then
        fail "Core package not found at ${core_config}. Build/install core first with ./build.sh."
    fi

    build_dir="$(mktemp -d "${TMPDIR:-/tmp}/plugin_uuid.XXXXXX")"

    (
        trap 'rm -rf "$build_dir"' EXIT

        cat > "${build_dir}/generate_uuid.cpp" <<'UUID_CPP'
#include <iostream>

#include "rm/reflection/uuid.hpp"

int main() {
    std::cout << rome::core::UUID().toString();
    return 0;
}
UUID_CPP

        cat > "${build_dir}/CMakeLists.txt" <<UUID_CMAKE
cmake_minimum_required(VERSION 3.14)
project(generate_uuid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(core CONFIG REQUIRED PATHS "${core_prefix}" NO_DEFAULT_PATH)

add_executable(generate_uuid "${build_dir}/generate_uuid.cpp")
target_link_libraries(generate_uuid PRIVATE core::core)
set_target_properties(
  generate_uuid
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${build_dir}/bin"
             BUILD_RPATH "${core_prefix}/lib")
UUID_CMAKE

        cmake -S "$build_dir" -B "${build_dir}/build" -DCMAKE_BUILD_TYPE=Release >/dev/null
        cmake --build "${build_dir}/build" >/dev/null

        "${build_dir}/bin/generate_uuid"
    )
}

generate_digest() {
    local core_dir="$1"
    local plugin_root="$2"
    local core_prefix
    local core_config
    local build_dir

    if [ ! -f "${core_dir}/CMakeLists.txt" ] || [ ! -d "${core_dir}/include/rm" ]; then
        fail "Expected core source directory at ${core_dir}."
    fi

    core_prefix="$(cd "${core_dir}/.." && pwd)/dist"
    core_config="${core_prefix}/lib/cmake/core/coreConfig.cmake"

    if [ ! -f "$core_config" ]; then
        fail "Core package not found at ${core_config}. Build/install core first with ./build.sh."
    fi

    build_dir="$(mktemp -d "${TMPDIR:-/tmp}/plugin_digest.XXXXXX")"

    (
        trap 'rm -rf "$build_dir"' EXIT

        cat > "${build_dir}/generate_digest.cpp" <<'DIGEST_CPP'
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

        cat > "${build_dir}/CMakeLists.txt" <<DIGEST_CMAKE
cmake_minimum_required(VERSION 3.14)
project(generate_digest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(core CONFIG REQUIRED PATHS "${core_prefix}" NO_DEFAULT_PATH)

add_executable(generate_digest "${build_dir}/generate_digest.cpp")
target_link_libraries(generate_digest PRIVATE core::core)
set_target_properties(
  generate_digest
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${build_dir}/bin"
             BUILD_RPATH "${core_prefix}/lib")
DIGEST_CMAKE

        cmake -S "$build_dir" -B "${build_dir}/build" -DCMAKE_BUILD_TYPE=Release >/dev/null
        cmake --build "${build_dir}/build" >/dev/null

        "${build_dir}/bin/generate_digest" "$plugin_root"
    )
}

if [ "$#" -eq 1 ]; then
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
    esac
fi

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    usage
    exit 1
fi

require_python
require_command date
require_command id

PLUGIN_NAME="$1"
OUTPUT_DIR="${2:-engine/plugins}"
AUTHOR="$(id -un 2>/dev/null || printf 'unknown')"
DATE="$(date +%F)"

case "$PLUGIN_NAME" in
    ""|*/*|*\\*|*.*)
        fail "Plugin name must be a directory name without slashes or dots."
        ;;
esac

if ! [[ "$PLUGIN_NAME" =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]]; then
    fail "Plugin name must match ^[A-Za-z_][A-Za-z0-9_]*$."
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

case "$OUTPUT_DIR" in
    ""|.|..|../*|*/..|*/../*)
        fail "Output directory must be a non-empty path under the repository."
        ;;
    /*)
        case "$OUTPUT_DIR" in
            "$REPO_ROOT"|"$REPO_ROOT"/*) ;;
            *) fail "Output directory must be under the repository: ${REPO_ROOT}" ;;
        esac
        OUTPUT_PARENT="$OUTPUT_DIR"
        ;;
    *)
        OUTPUT_PARENT="${REPO_ROOT}/${OUTPUT_DIR}"
        ;;
esac

PLUGIN_PARENT="$(resolve_output_parent "$REPO_ROOT" "$OUTPUT_DIR")"
PLUGIN_ROOT="${PLUGIN_PARENT}/${PLUGIN_NAME}"
PLUGIN_SRC="${PLUGIN_ROOT}/src"
PLUGIN_INCLUDE="${PLUGIN_ROOT}/include"

case "$PLUGIN_ROOT" in
    "$REPO_ROOT"/*) ;;
    *) fail "Resolved plugin path escapes repository root: ${PLUGIN_ROOT}" ;;
esac

if [ -e "$PLUGIN_ROOT" ]; then
    fail "Plugin root already exists: ${PLUGIN_ROOT}"
fi

VERSION_MAJOR=0
VERSION_MINOR=1
VERSION_PATCH=0
DESCRIPTION="${PLUGIN_NAME} plugin"

confirm_create

require_command cmake
require_command mktemp

UUID="$(generate_uuid "${REPO_ROOT}/core")"

mkdir -p "$PLUGIN_SRC" "$PLUGIN_INCLUDE"

cat > "${PLUGIN_ROOT}/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.14)

get_filename_component(PLUGIN_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
project(${PLUGIN_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CORE_PREFIX
    ""
    CACHE PATH "Path to core install prefix (dist folder)")
if(NOT CORE_PREFIX)
  message(FATAL_ERROR "CORE_PREFIX is not set. Configure with -DCORE_PREFIX=/path/to/dist")
endif()

find_package(core CONFIG REQUIRED PATHS "${CORE_PREFIX}" NO_DEFAULT_PATH)

file(GLOB_RECURSE PLUGIN_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
file(GLOB_RECURSE PLUGIN_HEADERS CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

list(FIND PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/plugin.cpp" PLUGIN_ENTRYPOINT_INDEX)
if(PLUGIN_ENTRYPOINT_INDEX EQUAL -1)
  message(FATAL_ERROR "Plugins must define src/plugin.cpp")
endif()

add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES} ${PLUGIN_HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE RM_ASSERTS_ON)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} PUBLIC core::core)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
  if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /O2)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -O3)
  endif()
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE RM_DEBUG_ON)
  if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
  endif()
endif()

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
             LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
             ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
             BUILD_RPATH "${CORE_PREFIX}/lib"
             INSTALL_RPATH "${CORE_PREFIX}/lib")
EOF

cat > "${PLUGIN_ROOT}/README.md" <<EOF
# ${PLUGIN_NAME}

Engine plugin.

## Layout

- \`plugin.json\` - plugin metadata
- \`CMakeLists.txt\` - standardized plugin build file
- \`build.sh\` - standalone plugin build helper
- \`clean.sh\` - standalone plugin clean helper
- \`src/plugin.cpp\` - required plugin entrypoints
- \`src/\` - plugin source files
- \`include/\` - plugin header files

## Build

\`\`\`sh
./build.sh --core /path/to/rome/core
\`\`\`

The build script writes the plugin binary to \`bin/\`.

## Clean

\`\`\`sh
./clean.sh
\`\`\`
EOF

cat > "${PLUGIN_ROOT}/build.sh" <<'EOF'
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
    echo -e "${YELLOW}Updated digest:${NC} ${digest}"
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
EOF
chmod +x "${PLUGIN_ROOT}/build.sh"

cat > "${PLUGIN_ROOT}/clean.sh" <<'EOF'
#!/usr/bin/env bash
set -euo pipefail

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

remove_path() {
    local path="$1"

    if [ -e "$path" ] || [ -L "$path" ]; then
        echo -e "${GREEN}Removing ${path}...${NC}"
        rm -rf "$path"
    else
        echo -e "${YELLOW}${path} does not exist. Skipping...${NC}"
    fi
}

echo -e "${GREEN}Cleaning plugin build artifacts...${NC}"

remove_path "build"
remove_path "bin"
remove_path ".cache"

echo -e "${GREEN}Plugin clean completed.${NC}"
EOF
chmod +x "${PLUGIN_ROOT}/clean.sh"

cat > "${PLUGIN_SRC}/plugin.cpp" <<'EOF'
#include "rm/ecs/ecs.hpp"
#include "rm/plugin/plugin.hpp"

using namespace rome;
using namespace rome::core;

extern "C" RM_PLUGIN_API void rome_load_plugin(ECS& ecs) {
    (void)ecs;
}

extern "C" RM_PLUGIN_API void rome_unload_plugin(ECS& ecs) {
    (void)ecs;
}
EOF

DIGEST="$(generate_digest "${REPO_ROOT}/core" "$PLUGIN_ROOT")"

PLUGIN_NAME="$PLUGIN_NAME" \
UUID="$UUID" \
AUTHOR="$AUTHOR" \
DATE="$DATE" \
DIGEST="$DIGEST" \
DESCRIPTION="$DESCRIPTION" \
VERSION_MAJOR="$VERSION_MAJOR" \
VERSION_MINOR="$VERSION_MINOR" \
VERSION_PATCH="$VERSION_PATCH" \
python3 - <<'PY' > "${PLUGIN_ROOT}/plugin.json"
import json
import os

plugin_name = os.environ["PLUGIN_NAME"]

data = {
    "name": plugin_name,
    "uuid": os.environ["UUID"],
    "version": {
        "major": int(os.environ["VERSION_MAJOR"]),
        "minor": int(os.environ["VERSION_MINOR"]),
        "patch": int(os.environ["VERSION_PATCH"]),
    },
    "author": os.environ["AUTHOR"],
    "date": os.environ["DATE"],
    "description": os.environ["DESCRIPTION"],
    "digest": os.environ["DIGEST"],
}

print(json.dumps(data, indent=2))
PY

echo -e "${GREEN}Created plugin template:${NC} ${PLUGIN_ROOT}"
echo -e "${YELLOW}Next:${NC} build it with ${PLUGIN_ROOT}/build.sh --core /path/to/rome/core"
