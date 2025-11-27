# git-reword-commit-cli

A cross-platform command-line tool for non-interactive git commit rewording. This tool automates the process of rewording git commits using a fully automated, non-interactive git rebase process with comprehensive error handling, verification steps, and rollback capability.

## Features

- 🚀 **Non-Interactive Rewording** - Reword git commits without manual interaction
- 🌍 **Cross-Platform** - Works on Windows, macOS, Linux, Android (Termux), and iOS
- 🔒 **Safe Operations** - Automatic backup tags and rollback capability
- ✅ **Pre-Flight Checks** - Validates repository state before operations
- 📦 **Multiple Build Systems** - CMake, Meson, and IDE project files
- 🔗 **Language Bindings** - Python, Java, Kotlin, and Dart bindings available

## Quick Start

### Building from Source

#### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.14 or later
- Git (for the actual reword operations)

#### Linux/macOS

```bash
# Configure and build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run
./build/git-reword-commit --help
```

#### Windows (Visual Studio)

```batch
# Using Visual Studio
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release

# Or using Ninja
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

#### Using Build Scripts

```bash
# Linux/macOS
./scripts/build.sh Release --ninja

# Windows
scripts\build.bat Release
```

### Using Conan Package Manager

```bash
# Install dependencies
conan install . --output-folder=build --build=missing

# Configure with Conan toolchain
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
cmake --build build
```

### Using Meson

```bash
meson setup build-meson --buildtype=release
meson compile -C build-meson
```

## Usage

```bash
# Basic usage
git-reword-commit <commit-hash> "New commit message"

# With verbose output
git-reword-commit --verbose abc1234 "Fix: correct typo in README"

# Working in a different repository
git-reword-commit -C /path/to/repo HEAD~2 "Refactor: improve performance"

# Keep backup tag after successful reword
git-reword-commit --keep-backup abc1234 "New message"

# Pre-flight checks only
git-reword-commit --check-only

# Rollback to a backup
git-reword-commit --rollback backup-before-reword-1234567890
```

### Options

| Option | Description |
|--------|-------------|
| `-h, --help` | Show help message |
| `-v, --version` | Show version information |
| `-C <path>` | Run in specified directory |
| `--keep-backup` | Keep backup tag after success |
| `--check-only` | Only perform pre-flight checks |
| `--verbose` | Show detailed progress |
| `--rollback <tag>` | Rollback to a backup tag |

## Cross-Platform Builds

### Android (NDK)

```bash
cmake -S . -B build-android \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/android.cmake \
    -DANDROID_NDK=$ANDROID_NDK_HOME \
    -DANDROID_ABI=arm64-v8a
cmake --build build-android
```

### iOS

```bash
cmake -S . -B build-ios \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/ios.cmake \
    -DIOS_PLATFORM=OS
cmake --build build-ios
```

### MinGW (Windows cross-compile from Linux)

```bash
cmake -S . -B build-mingw \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/mingw.cmake
cmake --build build-mingw
```

### Termux

```bash
# Build directly on Termux
pkg install cmake ninja clang
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## IDE Support

The project includes configuration files for:

- **CLion** - `.idea/` directory
- **Visual Studio** - `.vs/` directory with solution and project files
- **VS Code** - `.vscode/` with tasks, launch configs, and settings
- **Eclipse CDT** - `.project` and `.cproject` files
- **Qt Creator** - `git-reword-commit-cli.pro` file

## Language Bindings

### Python

```python
from git_reword import RewordEngine, RewordStatus

engine = RewordEngine("/path/to/repo")
result = engine.reword_commit("abc1234", "New commit message")

if result.status == RewordStatus.Success:
    print(f"Reworded: {result.old_commit_hash} -> {result.new_commit_hash}")
```

Build Python bindings:
```bash
cmake -S . -B build -DBUILD_PYTHON_BINDINGS=ON
cmake --build build
```

### Java/Kotlin

```java
try (RewordEngine engine = new RewordEngine("/path/to/repo")) {
    RewordResult result = engine.rewordCommit("abc1234", "New commit message");
    if (result.isSuccess()) {
        System.out.println("Success: " + result.getNewCommitHash());
    }
}
```

```kotlin
withRewordEngine("/path/to/repo") { engine ->
    val result = engine.rewordCommit("abc1234", "New message")
    println("Success: ${result.isSuccessful}")
}
```

Build Java bindings:
```bash
cmake -S . -B build -DBUILD_JAVA_BINDINGS=ON
cmake --build build
```

### Dart

```dart
final engine = RewordEngine('/path/to/repo');
final result = engine.rewordCommit('abc1234', 'New commit message');

if (result.isSuccess) {
  print('Reworded: ${result.oldCommitHash} -> ${result.newCommitHash}');
}

engine.dispose();
```

Build Dart bindings:
```bash
cmake -S . -B build -DBUILD_DART_BINDINGS=ON
cmake --build build
```

## Creating Distribution Package

```bash
# Create ZIP package
./scripts/package.sh 1.0.0

# Or using CMake/CPack
cd build
cpack -G ZIP
```

## Project Structure

```
git-reword-commit-cli/
├── CMakeLists.txt          # CMake build configuration
├── meson.build             # Meson build configuration
├── conanfile.py            # Conan package definition
├── conanfile.txt           # Conan dependencies
├── include/
│   └── git_reword/         # Public headers
├── src/                    # Source files
├── bindings/
│   ├── python/             # Python bindings (pybind11)
│   ├── java/               # Java/Kotlin bindings (JNI)
│   ├── kotlin/             # Kotlin extensions
│   └── dart/               # Dart bindings (FFI)
├── cmake/
│   └── toolchains/         # Cross-compilation toolchains
├── scripts/                # Build and packaging scripts
├── .vscode/                # VS Code configuration
├── .idea/                  # CLion/IntelliJ configuration
├── .vs/                    # Visual Studio files
├── .project                # Eclipse project
└── .cproject               # Eclipse CDT settings
```

## Error Handling

The tool provides comprehensive error handling:

| Status | Description |
|--------|-------------|
| `Success` | Operation completed successfully |
| `GitNotAvailable` | Git is not installed or not in PATH |
| `GitLfsNotAvailable` | Git LFS required but not available |
| `NotAGitRepository` | Current directory is not a git repository |
| `WorkingDirectoryDirty` | Uncommitted changes exist |
| `RebaseInProgress` | Another rebase is in progress |
| `CommitNotFound` | Specified commit doesn't exist |
| `RebaseFailed` | Rebase operation failed |
| `AmendFailed` | Failed to amend commit message |
| `RollbackFailed` | Rollback operation failed |

## License

This project is licensed under the MIT License - see the [LICENSE](../LICENSE) file for details.

## Author

**Baneeishaque K**

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
