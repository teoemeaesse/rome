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

echo -e "${GREEN}Cleaning generated build artifacts...${NC}"

remove_path "build"
remove_path "dist"
remove_path ".last_build"

remove_path "core/build"
remove_path "core/bin"
remove_path "core/.cache"

remove_path "engine/build"
remove_path "engine/bin"
remove_path "engine/.cache"

remove_path "sandbox/build"
remove_path "sandbox/bin"
remove_path "sandbox/.cache"

if [ -d "engine/plugins" ]; then
    while IFS= read -r -d '' plugin_dir; do
        plugin_clean="${plugin_dir}/clean.sh"

        if [ -f "$plugin_clean" ]; then
            echo -e "${GREEN}Cleaning ${plugin_dir}...${NC}"
            (cd "$plugin_dir" && bash ./clean.sh)
        fi
    done < <(find engine/plugins -mindepth 1 -maxdepth 1 -type d -print0)
else
    echo -e "${YELLOW}engine/plugins does not exist. Skipping plugin artifacts...${NC}"
fi

echo -e "${GREEN}Clean script completed.${NC}"
