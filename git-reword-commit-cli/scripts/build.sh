#!/bin/bash
# Build script for Linux/macOS
# Usage: ./scripts/build.sh [debug|release] [options]

set -e

BUILD_TYPE="${1:-Release}"
shift || true

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"

# Parse options
USE_NINJA=false
USE_CONAN=false
BUILD_PYTHON=false
BUILD_JAVA=false
BUILD_DART=false

for arg in "$@"; do
    case $arg in
        --ninja)
            USE_NINJA=true
            ;;
        --conan)
            USE_CONAN=true
            ;;
        --python)
            BUILD_PYTHON=true
            ;;
        --java)
            BUILD_JAVA=true
            ;;
        --dart)
            BUILD_DART=true
            ;;
        --all-bindings)
            BUILD_PYTHON=true
            BUILD_JAVA=true
            BUILD_DART=true
            ;;
        *)
            echo "Unknown option: $arg"
            exit 1
            ;;
    esac
done

echo "=== git-reword-commit-cli Build ==="
echo "Build type: $BUILD_TYPE"
echo "Build directory: $BUILD_DIR"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake options
CMAKE_OPTS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if $USE_NINJA && command -v ninja >/dev/null 2>&1; then
    CMAKE_OPTS="$CMAKE_OPTS -G Ninja"
    echo "Using Ninja generator"
fi

if $BUILD_PYTHON; then
    CMAKE_OPTS="$CMAKE_OPTS -DBUILD_PYTHON_BINDINGS=ON"
fi

if $BUILD_JAVA; then
    CMAKE_OPTS="$CMAKE_OPTS -DBUILD_JAVA_BINDINGS=ON"
fi

if $BUILD_DART; then
    CMAKE_OPTS="$CMAKE_OPTS -DBUILD_DART_BINDINGS=ON"
fi

# Run Conan if requested
if $USE_CONAN && command -v conan >/dev/null 2>&1; then
    echo "Installing Conan dependencies..."
    conan install "$PROJECT_DIR" --output-folder=. --build=missing
fi

# Configure
echo "Configuring..."
cmake $CMAKE_OPTS "$PROJECT_DIR"

# Build
echo "Building..."
cmake --build . --config "$BUILD_TYPE"

echo "Build complete!"
echo "Binary: $BUILD_DIR/git-reword-commit"
