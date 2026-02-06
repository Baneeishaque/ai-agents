# Android NDK toolchain file
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/android.cmake -DANDROID_ABI=arm64-v8a

# Check for NDK
if(NOT DEFINED ENV{ANDROID_NDK_HOME} AND NOT DEFINED ANDROID_NDK)
    message(FATAL_ERROR "ANDROID_NDK_HOME environment variable or ANDROID_NDK must be set")
endif()

if(NOT DEFINED ANDROID_NDK)
    set(ANDROID_NDK $ENV{ANDROID_NDK_HOME})
endif()

# Set Android platform
if(NOT DEFINED ANDROID_PLATFORM)
    set(ANDROID_PLATFORM android-21)
endif()

# Set ABI
if(NOT DEFINED ANDROID_ABI)
    set(ANDROID_ABI arm64-v8a)
endif()

# Set STL
if(NOT DEFINED ANDROID_STL)
    set(ANDROID_STL c++_shared)
endif()

# Include the official NDK toolchain
include(${ANDROID_NDK}/build/cmake/android.toolchain.cmake)

# Disable unsupported features
set(BUILD_PYTHON_BINDINGS OFF)
set(BUILD_JAVA_BINDINGS OFF)
set(BUILD_DART_BINDINGS OFF)
set(BUILD_TESTS OFF)
