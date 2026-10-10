#!/data/data/com.termux/files/usr/bin/bash

set -Eeuo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DIR="${PREFIX:-/data/data/com.termux/files/usr}"
BIN_DIR="$INSTALL_DIR/bin"
LIB_DIR="$INSTALL_DIR/lib/mathlang"
BUILD_DIR="$PROJECT_DIR/build"
RUNTIME="$BUILD_DIR/mathlang"

echo "=================================="
echo "       MathLang Installer"
echo "=================================="

if [[ ! -f "$PROJECT_DIR/CMakeLists.txt" ]]; then
    echo "Error: CMakeLists.txt not found."
    exit 1
fi

for tool in cmake clang++ make; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "Missing dependency: $tool"
        echo "Install it with:"
        echo "pkg install cmake clang make"
        exit 1
    fi
done

mkdir -p "$BUILD_DIR" "$BIN_DIR" "$LIB_DIR"

echo "[1/4] Configuring..."
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR"

echo "[2/4] Building..."
cmake --build "$BUILD_DIR" -j2

if [[ ! -x "$RUNTIME" ]]; then
    echo "Error: executable not found at $RUNTIME"
    echo "Check the executable name in CMakeLists.txt."
    exit 1
fi

echo "[3/4] Installing runtime..."
install -m 755 "$RUNTIME" "$LIB_DIR/mathlang"

echo "[4/4] Installing CLI..."
cat > "$BIN_DIR/mathlang" <<'CLI'
#!/data/data/com.termux/files/usr/bin/bash

set -e

RUNTIME="${PREFIX:-/data/data/com.termux/files/usr}/lib/mathlang/mathlang"

if [[ ! -x "$RUNTIME" ]]; then
    echo "Error: MathLang runtime is not installed."
    exit 1
fi

if [[ "$#" -eq 0 ]]; then
    echo "Usage: mathlang <file.mth>"
    exit 1
fi

exec "$RUNTIME" "$@"
CLI

chmod 755 "$BIN_DIR/mathlang"

echo
echo "MathLang installed successfully!"
echo "Try: mathlang /path/to/test.mth"
echo
echo "If the command is not found, restart Termux."