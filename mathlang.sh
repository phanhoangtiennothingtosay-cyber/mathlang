#!/usr/bin/env bash

set -Eeuo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RUNTIME="$PROJECT_DIR/build/mathlang"

if [[ ! -x "$RUNTIME" ]]; then
    echo "MathLang has not been built yet."
    echo "Build it with:"
    echo "  cmake -S \"$PROJECT_DIR\" -B \"$PROJECT_DIR/build\""
    echo "  cmake --build \"$PROJECT_DIR/build\" -j2"
    exit 1
fi

if [[ "$#" -eq 0 ]]; then
    echo "Usage: ./mathlang.sh <file.mth>"
    exit 1
fi

exec "$RUNTIME" "$@"