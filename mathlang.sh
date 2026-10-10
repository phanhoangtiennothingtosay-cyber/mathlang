#!/usr/bin/env bash

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$PROJECT_DIR/build/mathlang"

if [ ! -x "$BIN" ]; then
    echo "Error: MathLang executable not found."
    echo "Build the project first:"
    echo "  cmake -S \"$PROJECT_DIR\" -B \"$PROJECT_DIR/build\""
    echo "  cmake --build \"$PROJECT_DIR/build\" -j2"
    exit 1
fi

if [ "$#" -eq 0 ]; then
    echo "Usage: ./mathlang.sh <file.mth>"
    exit 1
fi

exec "$BIN" "$@"