/**
 * @file utils.hpp
 * @brief Utility functions for the git reword tool
 */

#ifndef GIT_REWORD_UTILS_HPP
#define GIT_REWORD_UTILS_HPP

#include "export.hpp"
#include <string>
#include <vector>
#include <optional>

namespace git_reword {
namespace utils {

/**
 * @brief Trim whitespace from both ends of a string
 */
GIT_REWORD_API std::string trim(const std::string& str);

/**
 * @brief Split a string by delimiter
 */
GIT_REWORD_API std::vector<std::string> split(const std::string& str, char delimiter);

/**
 * @brief Join strings with a delimiter
 */
GIT_REWORD_API std::string join(const std::vector<std::string>& parts, const std::string& delimiter);

/**
 * @brief Check if a string starts with a prefix
 */
GIT_REWORD_API bool starts_with(const std::string& str, const std::string& prefix);

/**
 * @brief Check if a string ends with a suffix
 */
GIT_REWORD_API bool ends_with(const std::string& str, const std::string& suffix);

/**
 * @brief Replace all occurrences of a substring
 */
GIT_REWORD_API std::string replace_all(const std::string& str, const std::string& from, const std::string& to);

/**
 * @brief Check if a file exists
 */
GIT_REWORD_API bool file_exists(const std::string& path);

/**
 * @brief Check if a directory exists
 */
GIT_REWORD_API bool directory_exists(const std::string& path);

/**
 * @brief Read entire file contents
 */
GIT_REWORD_API std::optional<std::string> read_file(const std::string& path);

/**
 * @brief Write content to a file
 */
GIT_REWORD_API bool write_file(const std::string& path, const std::string& content);

/**
 * @brief Delete a file
 */
GIT_REWORD_API bool delete_file(const std::string& path);

/**
 * @brief Delete a directory recursively
 */
GIT_REWORD_API bool delete_directory(const std::string& path);

/**
 * @brief Get current timestamp as string
 */
GIT_REWORD_API std::string get_timestamp();

/**
 * @brief Generate a unique backup tag name
 */
GIT_REWORD_API std::string generate_backup_tag_name();

} // namespace utils
} // namespace git_reword

#endif // GIT_REWORD_UTILS_HPP
