/**
 * @file git_operations.cpp
 * @brief Git operations implementation
 */

#include "git_reword/git_operations.hpp"
#include "git_reword/platform.hpp"
#include "git_reword/utils.hpp"

#include <sstream>
#include <algorithm>
#include <regex>

namespace git_reword {

namespace {
// Validate that a string is a safe git reference (commit hash, branch name, tag)
// Git refs can contain alphanumeric characters, -, _, /, ., and ~
// They cannot start with . or end with .lock, and cannot contain ..
bool is_safe_git_ref(const std::string& ref) {
    if (ref.empty() || ref.length() > 256) {
        return false;
    }
    
    // Check for shell-dangerous characters
    static const std::string dangerous_chars = ";|&$`\\!(){}[]<>\"'\n\r\t";
    for (char c : ref) {
        if (dangerous_chars.find(c) != std::string::npos) {
            return false;
        }
    }
    
    // Basic pattern for safe git refs
    static const std::regex safe_ref_pattern(R"(^[a-zA-Z0-9_.\-~^/]+$)");
    return std::regex_match(ref, safe_ref_pattern);
}
} // anonymous namespace

GitOperations::GitOperations(const std::string& repo_path)
    : repo_path_(repo_path) {
}

std::string GitOperations::get_git_dir() const {
    return repo_path_ + "/.git";
}

GitResult GitOperations::run_git_command(const std::string& args) const {
    GitResult result;
    
    std::string command = "git";
    if (repo_path_ != ".") {
        command += " -C \"" + repo_path_ + "\"";
    }
    command += " " + args;
    
    result.output = platform::execute_command(command, result.exit_code);
    result.success = (result.exit_code == 0);
    
    if (!result.success) {
        result.error = result.output;
    }
    
    return result;
}

bool GitOperations::is_git_available() const {
    return platform::command_exists("git");
}

bool GitOperations::is_git_lfs_available() const {
    return platform::command_exists("git-lfs");
}

bool GitOperations::is_git_repository() const {
    auto result = run_git_command("rev-parse --is-inside-work-tree");
    return result.success && utils::trim(result.output) == "true";
}

bool GitOperations::is_working_directory_clean() const {
    auto result = run_git_command("status --porcelain");
    return result.success && utils::trim(result.output).empty();
}

bool GitOperations::is_rebase_in_progress() const {
    std::string git_dir = get_git_dir();
    return utils::directory_exists(git_dir + "/rebase-merge") ||
           utils::directory_exists(git_dir + "/rebase-apply");
}

bool GitOperations::has_orphaned_rebase_state() const {
    if (!is_rebase_in_progress()) {
        return false;
    }
    
    // Check if there's actually a rebase in progress
    auto result = run_git_command("rebase --show-current-patch");
    return !result.success;
}

GitResult GitOperations::cleanup_orphaned_rebase_state() {
    GitResult result;
    std::string git_dir = get_git_dir();
    
    bool merge_deleted = true;
    bool apply_deleted = true;
    
    if (utils::directory_exists(git_dir + "/rebase-merge")) {
        merge_deleted = utils::delete_directory(git_dir + "/rebase-merge");
    }
    
    if (utils::directory_exists(git_dir + "/rebase-apply")) {
        apply_deleted = utils::delete_directory(git_dir + "/rebase-apply");
    }
    
    result.success = merge_deleted && apply_deleted;
    if (result.success) {
        result.output = "Cleaned up orphaned rebase state";
    } else {
        result.error = "Failed to clean up rebase state directories";
    }
    
    return result;
}

std::optional<std::string> GitOperations::get_head_commit() const {
    auto result = run_git_command("rev-parse HEAD");
    if (result.success) {
        return utils::trim(result.output);
    }
    return std::nullopt;
}

std::optional<CommitInfo> GitOperations::get_commit_info(const std::string& commit_ref) const {
    // Validate commit ref to prevent shell injection
    if (!is_safe_git_ref(commit_ref)) {
        return std::nullopt;
    }
    
    auto result = run_git_command("log -1 --format=\"%H|%h|%s|%an|%ai\" \"" + commit_ref + "\"");
    if (!result.success) {
        return std::nullopt;
    }
    
    auto parts = utils::split(utils::trim(result.output), '|');
    if (parts.size() < 5) {
        return std::nullopt;
    }
    
    CommitInfo info;
    info.hash = parts[0];
    info.short_hash = parts[1];
    info.message = parts[2];
    info.author = parts[3];
    info.date = parts[4];
    
    return info;
}

GitResult GitOperations::create_tag(const std::string& tag_name) {
    GitResult result;
    if (!is_safe_git_ref(tag_name)) {
        result.success = false;
        result.error = "Invalid tag name format";
        result.exit_code = -1;
        return result;
    }
    return run_git_command("tag \"" + tag_name + "\"");
}

GitResult GitOperations::delete_tag(const std::string& tag_name) {
    GitResult result;
    if (!is_safe_git_ref(tag_name)) {
        result.success = false;
        result.error = "Invalid tag name format";
        result.exit_code = -1;
        return result;
    }
    return run_git_command("tag -d \"" + tag_name + "\"");
}

bool GitOperations::verify_tag(const std::string& tag_name, const std::string& expected_commit) {
    // Validate inputs
    if (!is_safe_git_ref(tag_name) || !is_safe_git_ref(expected_commit)) {
        return false;
    }
    
    auto result = run_git_command("rev-parse \"" + tag_name + "\"");
    if (!result.success) {
        return false;
    }
    
    std::string tag_commit = utils::trim(result.output);
    
    // Get full hash for comparison if needed
    auto expected_result = run_git_command("rev-parse \"" + expected_commit + "\"");
    if (!expected_result.success) {
        return false;
    }
    
    std::string expected_full = utils::trim(expected_result.output);
    return tag_commit == expected_full;
}

GitResult GitOperations::start_reword_rebase(const std::string& commit_hash) {
    GitResult result;
    
    // Validate commit hash to prevent shell injection
    if (!is_safe_git_ref(commit_hash)) {
        result.success = false;
        result.error = "Invalid commit hash format";
        result.exit_code = -1;
        return result;
    }
    
    // Construct the sed command based on platform
    std::string sed_cmd;
    std::string short_hash = commit_hash.substr(0, 7);
    
#ifdef __APPLE__
    // macOS sed requires different syntax
    sed_cmd = "sed -i '' 's/^pick " + short_hash + "/reword " + short_hash + "/'";
#else
    // Linux/GNU sed
    sed_cmd = "sed -i 's/^pick " + short_hash + "/reword " + short_hash + "/'";
#endif
    
    std::string git_cmd = "GIT_SEQUENCE_EDITOR=\"" + sed_cmd + "\" git";
    if (repo_path_ != ".") {
        git_cmd += " -C \"" + repo_path_ + "\"";
    }
    git_cmd += " rebase -i \"" + commit_hash + "^\"";
    
    result.output = platform::execute_command(git_cmd, result.exit_code);
    result.success = (result.exit_code == 0);
    
    if (!result.success) {
        result.error = result.output;
    }
    
    return result;
}

GitResult GitOperations::amend_commit_message(const std::string& new_message) {
    // Escape quotes in the message
    std::string escaped_message = utils::replace_all(new_message, "\"", "\\\"");
    return run_git_command("commit --amend -m \"" + escaped_message + "\"");
}

GitResult GitOperations::continue_rebase() {
    return run_git_command("rebase --continue");
}

GitResult GitOperations::abort_rebase() {
    return run_git_command("rebase --abort");
}

GitResult GitOperations::reset_hard(const std::string& ref) {
    GitResult result;
    if (!is_safe_git_ref(ref)) {
        result.success = false;
        result.error = "Invalid git reference format";
        result.exit_code = -1;
        return result;
    }
    return run_git_command("reset --hard \"" + ref + "\"");
}

std::vector<CommitInfo> GitOperations::get_log(int count) const {
    std::vector<CommitInfo> commits;
    
    auto result = run_git_command(
        "log -" + std::to_string(count) + 
        " --format=\"%H|%h|%s|%an|%ai\""
    );
    
    if (!result.success) {
        return commits;
    }
    
    auto lines = utils::split(result.output, '\n');
    for (const auto& line : lines) {
        std::string trimmed = utils::trim(line);
        if (trimmed.empty()) continue;
        
        auto parts = utils::split(trimmed, '|');
        if (parts.size() >= 5) {
            CommitInfo info;
            info.hash = parts[0];
            info.short_hash = parts[1];
            info.message = parts[2];
            info.author = parts[3];
            info.date = parts[4];
            commits.push_back(info);
        }
    }
    
    return commits;
}

} // namespace git_reword
