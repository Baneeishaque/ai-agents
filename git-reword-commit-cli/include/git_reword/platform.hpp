/**
 * @file platform.hpp
 * @brief Platform-specific utilities and definitions
 */

#ifndef GIT_REWORD_PLATFORM_HPP
#define GIT_REWORD_PLATFORM_HPP

#include "export.hpp"
#include <string>
#include <vector>

namespace git_reword {
namespace platform {

/**
 * @brief Check if running on Windows
 */
GIT_REWORD_API bool is_windows();

/**
 * @brief Check if running on macOS
 */
GIT_REWORD_API bool is_macos();

/**
 * @brief Check if running on Linux
 */
GIT_REWORD_API bool is_linux();

/**
 * @brief Check if running on Android
 */
GIT_REWORD_API bool is_android();

/**
 * @brief Check if running on iOS
 */
GIT_REWORD_API bool is_ios();

/**
 * @brief Check if running in Termux environment
 */
GIT_REWORD_API bool is_termux();

/**
 * @brief Get the platform name as a string
 */
GIT_REWORD_API std::string get_platform_name();

/**
 * @brief Get the path separator for the current platform
 */
GIT_REWORD_API char get_path_separator();

/**
 * @brief Execute a command and return the output
 * @param command The command to execute
 * @param exit_code Output parameter for the exit code
 * @return The command output
 */
GIT_REWORD_API std::string execute_command(const std::string& command, int& exit_code);

/**
 * @brief Check if a command exists in PATH
 * @param command The command to check
 * @return true if the command exists
 */
GIT_REWORD_API bool command_exists(const std::string& command);

/**
 * @brief Get environment variable value
 * @param name The environment variable name
 * @return The value or empty string if not set
 */
GIT_REWORD_API std::string get_env(const std::string& name);

/**
 * @brief Get the current working directory
 */
GIT_REWORD_API std::string get_current_directory();

/**
 * @brief Change the current working directory
 */
GIT_REWORD_API bool set_current_directory(const std::string& path);

} // namespace platform
} // namespace git_reword

#endif // GIT_REWORD_PLATFORM_HPP
