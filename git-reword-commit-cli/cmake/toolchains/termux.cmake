# Termux toolchain file
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/termux.cmake
# This is for building directly on Termux, not cross-compiling

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Termux uses clang by default
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Termux prefix
if(DEFINED ENV{PREFIX})
    set(TERMUX_PREFIX $ENV{PREFIX})
else()
    set(TERMUX_PREFIX /data/data/com.termux/files/usr)
endif()

# Include and library paths
set(CMAKE_FIND_ROOT_PATH ${TERMUX_PREFIX})
include_directories(${TERMUX_PREFIX}/include)
link_directories(${TERMUX_PREFIX}/lib)

# Termux-specific definitions
add_definitions(-D__ANDROID__)
add_definitions(-DTERMUX)
