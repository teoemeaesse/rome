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
    fi
}

echo -e "${GREEN}Cleaning plugin build artifacts...${NC}"

remove_path "build"
remove_path "bin"
remove_path ".cache"

echo -e "${GREEN}Plugin clean completed.${NC}"
