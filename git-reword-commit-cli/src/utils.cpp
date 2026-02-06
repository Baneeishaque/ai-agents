/**
 * @file utils.cpp
 * @brief Utility function implementations
 */

#include "git_reword/utils.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
    #include <windows.h>
    #include <shellapi.h>
    #include <direct.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
    #include <dirent.h>
#endif

namespace git_reword {
namespace utils {

std::string trim(const std::string& str) {
    const auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        return "";
    }
    const auto end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

std::string join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            result += delimiter;
        }
        result += parts[i];
    }
    return result;
}

bool starts_with(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) {
        return false;
    }
    return str.compare(0, prefix.length(), prefix) == 0;
}

bool ends_with(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::string replace_all(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return str;
    }
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

bool file_exists(const std::string& path) {
#ifdef _WIN32
    DWORD attribs = GetFileAttributesA(path.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
#endif
}

bool directory_exists(const std::string& path) {
#ifdef _WIN32
    DWORD attribs = GetFileAttributesA(path.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode));
#endif
}

std::optional<std::string> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return std::nullopt;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        return false;
    }
    file << content;
    return file.good();
}

bool delete_file(const std::string& path) {
    return std::remove(path.c_str()) == 0;
}

bool delete_directory(const std::string& path) {
#ifdef _WIN32
    // Windows implementation using shell API
    std::string double_null_path = path + '\0' + '\0';
    SHFILEOPSTRUCTA fileOp = {
        NULL,
        FO_DELETE,
        double_null_path.c_str(),
        NULL,
        FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
        FALSE,
        NULL,
        NULL
    };
    return SHFileOperationA(&fileOp) == 0;
#else
    // Unix implementation - recursive delete
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return false;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") {
            continue;
        }
        
        std::string full_path = path + "/" + name;
        struct stat st;
        if (stat(full_path.c_str(), &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                delete_directory(full_path);
            } else {
                unlink(full_path.c_str());
            }
        }
    }
    closedir(dir);
    return rmdir(path.c_str()) == 0;
#endif
}

std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    return std::to_string(time);
}

std::string generate_backup_tag_name() {
    return "backup-before-reword-" + get_timestamp();
}

} // namespace utils
} // namespace git_reword
