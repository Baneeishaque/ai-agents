@echo off
REM Build script for Windows
REM Usage: build.bat [Debug|Release] [options]

setlocal enabledelayedexpansion

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set SCRIPT_DIR=%~dp0
set PROJECT_DIR=%SCRIPT_DIR%..
set BUILD_DIR=%PROJECT_DIR%\build

set USE_NINJA=false
set USE_CONAN=false
set BUILD_PYTHON=false
set BUILD_JAVA=false
set BUILD_DART=false

REM Parse options
:parse_args
if "%2"=="" goto done_parsing
if "%2"=="--ninja" set USE_NINJA=true
if "%2"=="--conan" set USE_CONAN=true
if "%2"=="--python" set BUILD_PYTHON=true
if "%2"=="--java" set BUILD_JAVA=true
if "%2"=="--dart" set BUILD_DART=true
if "%2"=="--all-bindings" (
    set BUILD_PYTHON=true
    set BUILD_JAVA=true
    set BUILD_DART=true
)
shift
goto parse_args
:done_parsing

echo === git-reword-commit-cli Build ===
echo Build type: %BUILD_TYPE%
echo Build directory: %BUILD_DIR%

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Configure CMake options
set CMAKE_OPTS=-DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if "%USE_NINJA%"=="true" (
    where ninja >nul 2>nul
    if %errorlevel%==0 (
        set CMAKE_OPTS=%CMAKE_OPTS% -G Ninja
        echo Using Ninja generator
    )
) else (
    REM Use Visual Studio generator by default
    set CMAKE_OPTS=%CMAKE_OPTS% -G "Visual Studio 17 2022" -A x64
)

if "%BUILD_PYTHON%"=="true" set CMAKE_OPTS=%CMAKE_OPTS% -DBUILD_PYTHON_BINDINGS=ON
if "%BUILD_JAVA%"=="true" set CMAKE_OPTS=%CMAKE_OPTS% -DBUILD_JAVA_BINDINGS=ON
if "%BUILD_DART%"=="true" set CMAKE_OPTS=%CMAKE_OPTS% -DBUILD_DART_BINDINGS=ON

REM Run Conan if requested
if "%USE_CONAN%"=="true" (
    where conan >nul 2>nul
    if %errorlevel%==0 (
        echo Installing Conan dependencies...
        conan install "%PROJECT_DIR%" --output-folder=. --build=missing
    )
)

REM Configure
echo Configuring...
cmake %CMAKE_OPTS% "%PROJECT_DIR%"

REM Build
echo Building...
cmake --build . --config %BUILD_TYPE%

echo Build complete!
echo Binary: %BUILD_DIR%\%BUILD_TYPE%\git-reword-commit.exe

endlocal
