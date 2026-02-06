/**
 * @file reword_engine.hpp
 * @brief Main engine for git commit rewording
 */

#ifndef GIT_REWORD_REWORD_ENGINE_HPP
#define GIT_REWORD_REWORD_ENGINE_HPP

#include "export.hpp"
#include "git_operations.hpp"
#include <string>
#include <functional>
#include <memory>

namespace git_reword {

/**
 * @brief Result status for reword operations
 */
enum class GIT_REWORD_API RewordStatus {
    Success,
    GitNotAvailable,
    GitLfsNotAvailable,
    NotAGitRepository,
    WorkingDirectoryDirty,
    RebaseInProgress,
    CommitNotFound,
    RebaseFailed,
    AmendFailed,
    RollbackFailed,
    UnknownError
};

/**
 * @brief Convert RewordStatus to string
 */
GIT_REWORD_API std::string status_to_string(RewordStatus status);

/**
 * @brief Result of a reword operation
 */
struct GIT_REWORD_API RewordResult {
    RewordStatus status;
    std::string message;
    std::string old_commit_hash;
    std::string new_commit_hash;
    std::string backup_tag;
};

/**
 * @brief Callback type for progress reporting
 */
using ProgressCallback = std::function<void(const std::string& step, const std::string& details)>;

/**
 * @brief Main engine for rewording git commits
 */
class GIT_REWORD_API RewordEngine {
public:
    /**
     * @brief Construct the reword engine
     * @param repo_path Path to the git repository
     */
    explicit RewordEngine(const std::string& repo_path = ".");

    /**
     * @brief Set progress callback for reporting steps
     */
    void set_progress_callback(ProgressCallback callback);

    /**
     * @brief Perform pre-flight checks
     * @return Status indicating if checks passed
     */
    RewordResult preflight_checks();

    /**
     * @brief Reword a commit with a new message
     * @param commit_hash The commit hash to reword
     * @param new_message The new commit message
     * @param keep_backup Whether to keep the backup tag on success
     * @return Result of the reword operation
     */
    RewordResult reword_commit(
        const std::string& commit_hash,
        const std::string& new_message,
        bool keep_backup = false
    );

    /**
     * @brief Rollback to the backup tag
     * @param backup_tag The backup tag to restore to
     */
    RewordResult rollback(const std::string& backup_tag);

    /**
     * @brief Get the underlying git operations object
     */
    const GitOperations& get_git_operations() const { return git_ops_; }

    /**
     * @brief Get version string
     */
    static std::string get_version();

private:
    GitOperations git_ops_;
    ProgressCallback progress_callback_;

    void report_progress(const std::string& step, const std::string& details = "");
};

} // namespace git_reword

#endif // GIT_REWORD_REWORD_ENGINE_HPP
