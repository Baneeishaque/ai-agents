cmake_minimum_required(VERSION 3.14)

# Project configuration
project(git-reword-commit-cli
    VERSION 1.0.0
    DESCRIPTION "Cross-platform CLI tool for non-interactive git commit rewording"
    HOMEPAGE_URL "https://github.com/Baneeishaque/AI-Agents"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Option to build as shared library for bindings
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
option(BUILD_JAVA_BINDINGS "Build Java/Kotlin bindings (JNI)" OFF)
option(BUILD_DART_BINDINGS "Build Dart bindings (FFI)" OFF)
option(BUILD_TESTS "Build tests" ON)

# Platform detection
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM_ANDROID ON)
    message(STATUS "Building for Android")
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(PLATFORM_IOS ON)
    message(STATUS "Building for iOS")
elseif(WIN32)
    set(PLATFORM_WINDOWS ON)
    message(STATUS "Building for Windows")
elseif(APPLE)
    set(PLATFORM_MACOS ON)
    message(STATUS "Building for macOS")
elseif(UNIX)
    set(PLATFORM_LINUX ON)
    message(STATUS "Building for Linux/Unix")
endif()

# Compiler detection and settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Using MSVC compiler")
    add_compile_options(/W4 /WX- /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC compiler")
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(MINGW)
        message(STATUS "Using MinGW")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Using Clang compiler")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Core library
set(CORE_SOURCES
    src/git_operations.cpp
    src/reword_engine.cpp
    src/utils.cpp
    src/platform.cpp
)

set(CORE_HEADERS
    include/git_reword/git_operations.hpp
    include/git_reword/reword_engine.hpp
    include/git_reword/utils.hpp
    include/git_reword/platform.hpp
    include/git_reword/export.hpp
)

# Create core library (can be static or shared based on BUILD_SHARED_LIBS)
add_library(git_reword_core ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(git_reword_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Export macros for shared library
if(BUILD_SHARED_LIBS)
    target_compile_definitions(git_reword_core PRIVATE GIT_REWORD_EXPORTS)
    target_compile_definitions(git_reword_core PUBLIC GIT_REWORD_SHARED)
endif()

# Main CLI executable
add_executable(git-reword-commit src/main.cpp)
target_link_libraries(git-reword-commit PRIVATE git_reword_core)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(git_reword_core PRIVATE shlwapi)
endif()

# Conan integration (if conan is used)
if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
elseif(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
endif()

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(bindings/python)
endif()

# Java/Kotlin bindings (JNI)
if(BUILD_JAVA_BINDINGS)
    add_subdirectory(bindings/java)
endif()

# Dart bindings
if(BUILD_DART_BINDINGS)
    add_subdirectory(bindings/dart)
endif()

# Testing
if(BUILD_TESTS)
    enable_testing()
    # Tests can be added here
endif()

# Installation
include(GNUInstallDirs)

install(TARGETS git-reword-commit git_reword_core
    EXPORT git-reword-commit-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/git_reword
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export for find_package
install(EXPORT git-reword-commit-targets
    FILE git-reword-commit-targets.cmake
    NAMESPACE GitReword::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/git-reword-commit
)

# CPack configuration for packaging
include(CPack)
set(CPACK_PACKAGE_NAME "git-reword-commit-cli")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "Baneeishaque K")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# ZIP generator for all platforms
set(CPACK_GENERATOR "ZIP")
if(WIN32)
    list(APPEND CPACK_GENERATOR "NSIS")
elseif(APPLE)
    list(APPEND CPACK_GENERATOR "DragNDrop")
elseif(UNIX)
    list(APPEND CPACK_GENERATOR "TGZ" "DEB" "RPM")
endif()
