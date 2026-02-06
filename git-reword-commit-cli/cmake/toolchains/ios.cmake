# iOS toolchain file
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/ios.cmake -DIOS_PLATFORM=OS

# iOS platform options:
# OS - Build for iOS device
# SIMULATOR - Build for iOS simulator
# WATCHOS - Build for watchOS device
# SIMULATOR_WATCHOS - Build for watchOS simulator
# TVOS - Build for tvOS device
# SIMULATOR_TVOS - Build for tvOS simulator

if(NOT DEFINED IOS_PLATFORM)
    set(IOS_PLATFORM OS)
endif()

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_SYSROOT iphoneos)

# Set deployment target
if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)
endif()

# Set architectures based on platform
if(IOS_PLATFORM STREQUAL "OS")
    set(CMAKE_OSX_ARCHITECTURES arm64)
    set(CMAKE_OSX_SYSROOT iphoneos)
elseif(IOS_PLATFORM STREQUAL "SIMULATOR")
    set(CMAKE_OSX_ARCHITECTURES x86_64 arm64)
    set(CMAKE_OSX_SYSROOT iphonesimulator)
endif()

# Compiler settings
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# Disable features not supported on iOS
set(BUILD_PYTHON_BINDINGS OFF)
set(BUILD_JAVA_BINDINGS OFF)
set(BUILD_DART_BINDINGS OFF)
set(BUILD_TESTS OFF)

# iOS-specific flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fembed-bitcode")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fembed-bitcode")
