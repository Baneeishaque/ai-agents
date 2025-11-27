/**
 * @file platform.cpp
 * @brief Platform-specific implementations
 */

#include "git_reword/platform.hpp"

#include <cstdlib>
#include <array>
#include <memory>
#include <stdexcept>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define popen _popen
    #define pclose _pclose
    #define getcwd _getcwd
    #define chdir _chdir
#else
    #include <unistd.h>
    #include <sys/stat.h>
#endif

namespace git_reword {
namespace platform {

bool is_windows() {
#ifdef _WIN32
    return true;
#else
    return false;
#endif
}

bool is_macos() {
#ifdef __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC && !TARGET_OS_IPHONE
        return true;
    #endif
#endif
    return false;
}

bool is_linux() {
#if defined(__linux__) && !defined(__ANDROID__)
    return true;
#else
    return false;
#endif
}

bool is_android() {
#ifdef __ANDROID__
    return true;
#else
    return false;
#endif
}

bool is_ios() {
#ifdef __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        return true;
    #endif
#endif
    return false;
}

bool is_termux() {
    // Check for Termux-specific environment variable
    const char* prefix = std::getenv("PREFIX");
    if (prefix && std::string(prefix).find("/data/data/com.termux") != std::string::npos) {
        return true;
    }
    return false;
}

std::string get_platform_name() {
    if (is_windows()) return "Windows";
    if (is_macos()) return "macOS";
    if (is_ios()) return "iOS";
    if (is_android()) return "Android";
    if (is_termux()) return "Termux";
    if (is_linux()) return "Linux";
    return "Unknown";
}

char get_path_separator() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}

std::string execute_command(const std::string& command, int& exit_code) {
    std::array<char, 128> buffer;
    std::string result;
    
    // Redirect stderr to stdout
    std::string cmd = command + " 2>&1";
    
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        exit_code = -1;
        return "Failed to execute command";
    }
    
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    exit_code = pclose(pipe.release());
#ifndef _WIN32
    // On Unix, pclose returns the exit status shifted
    if (WIFEXITED(exit_code)) {
        exit_code = WEXITSTATUS(exit_code);
    }
#endif
    
    return result;
}

bool command_exists(const std::string& command) {
    int exit_code;
#ifdef _WIN32
    execute_command("where " + command + " >nul 2>&1", exit_code);
#else
    execute_command("command -v " + command + " >/dev/null 2>&1", exit_code);
#endif
    return exit_code == 0;
}

std::string get_env(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : "";
}

std::string get_current_directory() {
    char buffer[4096];
    if (getcwd(buffer, sizeof(buffer))) {
        return std::string(buffer);
    }
    return "";
}

bool set_current_directory(const std::string& path) {
    return chdir(path.c_str()) == 0;
}

} // namespace platform
} // namespace git_reword
