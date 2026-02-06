/**
 * @file git_operations.hpp
 * @brief Git operation wrappers for the reword tool
 */

#ifndef GIT_REWORD_GIT_OPERATIONS_HPP
#define GIT_REWORD_GIT_OPERATIONS_HPP

#include "export.hpp"
#include <string>
#include <vector>
#include <optional>

namespace git_reword {

/**
 * @brief Result of a git operation
 */
struct GIT_REWORD_API GitResult {
    bool success;
    std::string output;
    std::string error;
    int exit_code;
};

/**
 * @brief Commit information
 */
struct GIT_REWORD_API CommitInfo {
    std::string hash;
    std::string short_hash;
    std::string message;
    std::string author;
    std::string date;
};

/**
 * @brief Git operations wrapper class
 */
class GIT_REWORD_API GitOperations {
public:
    /**
     * @brief Construct GitOperations for a specific repository path
     * @param repo_path Path to the git repository (default: current directory)
     */
    explicit GitOperations(const std::string& repo_path = ".");

    /**
     * @brief Check if git is available
     */
    bool is_git_available() const;

    /**
     * @brief Check if git-lfs is available
     */
    bool is_git_lfs_available() const;

    /**
     * @brief Check if current directory is a git repository
     */
    bool is_git_repository() const;

    /**
     * @brief Check if working directory is clean
     */
    bool is_working_directory_clean() const;

    /**
     * @brief Check if a rebase is in progress
     */
    bool is_rebase_in_progress() const;

    /**
     * @brief Check for orphaned rebase state
     */
    bool has_orphaned_rebase_state() const;

    /**
     * @brief Clean up orphaned rebase state
     */
    GitResult cleanup_orphaned_rebase_state();

    /**
     * @brief Get current HEAD commit hash
     */
    std::optional<std::string> get_head_commit() const;

    /**
     * @brief Get commit information
     */
    std::optional<CommitInfo> get_commit_info(const std::string& commit_ref) const;

    /**
     * @brief Create a backup tag
     * @param tag_name Name of the tag
     * @return Result of the operation
     */
    GitResult create_tag(const std::string& tag_name);

    /**
     * @brief Delete a tag
     * @param tag_name Name of the tag to delete
     */
    GitResult delete_tag(const std::string& tag_name);

    /**
     * @brief Verify tag points to expected commit
     */
    bool verify_tag(const std::string& tag_name, const std::string& expected_commit);

    /**
     * @brief Start a non-interactive rebase to reword a commit
     * @param commit_hash The commit to reword
     * @return Result of the operation
     */
    GitResult start_reword_rebase(const std::string& commit_hash);

    /**
     * @brief Amend the current commit with a new message
     * @param new_message The new commit message
     */
    GitResult amend_commit_message(const std::string& new_message);

    /**
     * @brief Continue the rebase
     */
    GitResult continue_rebase();

    /**
     * @brief Abort the current rebase
     */
    GitResult abort_rebase();

    /**
     * @brief Reset to a specific commit/tag
     * @param ref The reference to reset to
     */
    GitResult reset_hard(const std::string& ref);

    /**
     * @brief Get recent commit log
     * @param count Number of commits to show
     */
    std::vector<CommitInfo> get_log(int count = 5) const;

    /**
     * @brief Get the repository path
     */
    const std::string& get_repo_path() const { return repo_path_; }

private:
    std::string repo_path_;
    
    GitResult run_git_command(const std::string& args) const;
    std::string get_git_dir() const;
};

} // namespace git_reword

#endif // GIT_REWORD_GIT_OPERATIONS_HPP
