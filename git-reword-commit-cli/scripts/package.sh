#!/bin/bash
# Create ZIP package for distribution
# Usage: ./scripts/package.sh [version]

set -e

VERSION="${1:-1.0.0}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"
PACKAGE_DIR="$PROJECT_DIR/package"

echo "=== Creating Package v$VERSION ==="

# Clean package directory
rm -rf "$PACKAGE_DIR"
mkdir -p "$PACKAGE_DIR/git-reword-commit-cli-$VERSION"

# Build if not already built
if [ ! -f "$BUILD_DIR/git-reword-commit" ]; then
    echo "Building project first..."
    "$SCRIPT_DIR/build.sh" Release
fi

# Copy files
cd "$PACKAGE_DIR/git-reword-commit-cli-$VERSION"

# Binary
cp "$BUILD_DIR/git-reword-commit"* . 2>/dev/null || true

# Documentation
cp "$PROJECT_DIR/README.md" .
cp "$PROJECT_DIR/../LICENSE" . 2>/dev/null || true

# Create archive
cd "$PACKAGE_DIR"
ZIP_NAME="git-reword-commit-cli-$VERSION-$(uname -s)-$(uname -m).zip"
zip -r "$ZIP_NAME" "git-reword-commit-cli-$VERSION"

echo "Package created: $PACKAGE_DIR/$ZIP_NAME"
