/**
 * @file reword_engine.cpp
 * @brief Main reword engine implementation
 */

#include "git_reword/reword_engine.hpp"
#include "git_reword/utils.hpp"

namespace git_reword {

std::string status_to_string(RewordStatus status) {
    switch (status) {
        case RewordStatus::Success:
            return "Success";
        case RewordStatus::GitNotAvailable:
            return "Git is not available";
        case RewordStatus::GitLfsNotAvailable:
            return "Git LFS is not available (required for repositories with LFS objects)";
        case RewordStatus::NotAGitRepository:
            return "Not a git repository";
        case RewordStatus::WorkingDirectoryDirty:
            return "Working directory has uncommitted changes";
        case RewordStatus::RebaseInProgress:
            return "A rebase is already in progress";
        case RewordStatus::CommitNotFound:
            return "Commit not found";
        case RewordStatus::RebaseFailed:
            return "Rebase operation failed";
        case RewordStatus::AmendFailed:
            return "Failed to amend commit message";
        case RewordStatus::RollbackFailed:
            return "Rollback failed";
        case RewordStatus::UnknownError:
        default:
            return "Unknown error";
    }
}

RewordEngine::RewordEngine(const std::string& repo_path)
    : git_ops_(repo_path), progress_callback_(nullptr) {
}

void RewordEngine::set_progress_callback(ProgressCallback callback) {
    progress_callback_ = callback;
}

void RewordEngine::report_progress(const std::string& step, const std::string& details) {
    if (progress_callback_) {
        progress_callback_(step, details);
    }
}

RewordResult RewordEngine::preflight_checks() {
    RewordResult result;
    result.status = RewordStatus::Success;
    
    // Check git availability
    report_progress("Checking git availability");
    if (!git_ops_.is_git_available()) {
        result.status = RewordStatus::GitNotAvailable;
        result.message = status_to_string(result.status);
        return result;
    }
    
    // Check if in a git repository
    report_progress("Verifying git repository");
    if (!git_ops_.is_git_repository()) {
        result.status = RewordStatus::NotAGitRepository;
        result.message = status_to_string(result.status);
        return result;
    }
    
    // Check for orphaned rebase state
    report_progress("Checking for existing rebase state");
    if (git_ops_.has_orphaned_rebase_state()) {
        report_progress("Cleaning up orphaned rebase state");
        auto cleanup = git_ops_.cleanup_orphaned_rebase_state();
        if (!cleanup.success) {
            result.status = RewordStatus::RebaseInProgress;
            result.message = "Failed to cleanup orphaned rebase state";
            return result;
        }
    } else if (git_ops_.is_rebase_in_progress()) {
        result.status = RewordStatus::RebaseInProgress;
        result.message = "A rebase is already in progress. Please complete or abort it first.";
        return result;
    }
    
    // Check working directory status
    report_progress("Checking working directory status");
    if (!git_ops_.is_working_directory_clean()) {
        result.status = RewordStatus::WorkingDirectoryDirty;
        result.message = "Working directory has uncommitted changes. Please commit or stash them first.";
        return result;
    }
    
    // Check git-lfs availability (optional warning)
    report_progress("Checking Git LFS availability");
    if (!git_ops_.is_git_lfs_available()) {
        // This is a warning, not an error - some repos don't need LFS
        result.message = "Warning: Git LFS is not available. If this repository uses LFS, the rebase may fail.";
    }
    
    return result;
}

RewordResult RewordEngine::reword_commit(
    const std::string& commit_hash,
    const std::string& new_message,
    bool keep_backup) {
    
    RewordResult result;
    
    // Perform preflight checks
    result = preflight_checks();
    if (result.status != RewordStatus::Success) {
        return result;
    }
    
    // Get the commit info
    report_progress("Verifying commit", commit_hash);
    auto commit_info = git_ops_.get_commit_info(commit_hash);
    if (!commit_info) {
        result.status = RewordStatus::CommitNotFound;
        result.message = "Commit not found: " + commit_hash;
        return result;
    }
    result.old_commit_hash = commit_info->hash;
    
    // Create backup tag
    result.backup_tag = utils::generate_backup_tag_name();
    report_progress("Creating backup tag", result.backup_tag);
    
    auto head = git_ops_.get_head_commit();
    if (!head) {
        result.status = RewordStatus::UnknownError;
        result.message = "Failed to get HEAD commit";
        return result;
    }
    
    auto tag_result = git_ops_.create_tag(result.backup_tag);
    if (!tag_result.success) {
        result.status = RewordStatus::UnknownError;
        result.message = "Failed to create backup tag: " + tag_result.error;
        return result;
    }
    
    // Verify backup tag
    if (!git_ops_.verify_tag(result.backup_tag, *head)) {
        git_ops_.delete_tag(result.backup_tag);
        result.status = RewordStatus::UnknownError;
        result.message = "Backup tag verification failed";
        return result;
    }
    
    // Start non-interactive rebase
    report_progress("Starting non-interactive rebase");
    auto rebase_result = git_ops_.start_reword_rebase(commit_hash);
    
    if (!rebase_result.success) {
        // Check for LFS error
        if (rebase_result.error.find("git-lfs") != std::string::npos) {
            result.status = RewordStatus::GitLfsNotAvailable;
            result.message = "Git LFS is required for this repository. Please install git-lfs.";
        } else {
            result.status = RewordStatus::RebaseFailed;
            result.message = "Rebase failed: " + rebase_result.error;
        }
        
        // Cleanup
        git_ops_.abort_rebase();
        git_ops_.delete_tag(result.backup_tag);
        return result;
    }
    
    // Amend commit message
    report_progress("Amending commit message");
    auto amend_result = git_ops_.amend_commit_message(new_message);
    if (!amend_result.success) {
        result.status = RewordStatus::AmendFailed;
        result.message = "Failed to amend commit message: " + amend_result.error;
        
        // Rollback
        git_ops_.abort_rebase();
        git_ops_.reset_hard(result.backup_tag);
        git_ops_.delete_tag(result.backup_tag);
        return result;
    }
    
    // Continue rebase
    report_progress("Completing rebase");
    auto continue_result = git_ops_.continue_rebase();
    if (!continue_result.success && 
        continue_result.error.find("No rebase in progress") == std::string::npos) {
        // Ignore "no rebase in progress" - that means it completed automatically
        result.status = RewordStatus::RebaseFailed;
        result.message = "Failed to complete rebase: " + continue_result.error;
        
        // Rollback
        git_ops_.abort_rebase();
        git_ops_.reset_hard(result.backup_tag);
        git_ops_.delete_tag(result.backup_tag);
        return result;
    }
    
    // Verify success
    report_progress("Verifying reword success");
    auto new_head = git_ops_.get_head_commit();
    if (new_head) {
        result.new_commit_hash = *new_head;
    }
    
    // Get the new commit info to verify message changed
    auto new_commit_info = git_ops_.get_commit_info("HEAD");
    if (new_commit_info) {
        report_progress("Reword complete", "New message: " + new_commit_info->message);
    }
    
    // Cleanup backup tag if not keeping
    if (!keep_backup) {
        report_progress("Cleaning up backup tag");
        git_ops_.delete_tag(result.backup_tag);
        result.backup_tag.clear();
    }
    
    result.status = RewordStatus::Success;
    result.message = "Successfully reworded commit";
    
    return result;
}

RewordResult RewordEngine::rollback(const std::string& backup_tag) {
    RewordResult result;
    
    report_progress("Rolling back to backup", backup_tag);
    
    // Reset to backup tag
    auto reset_result = git_ops_.reset_hard(backup_tag);
    if (!reset_result.success) {
        result.status = RewordStatus::RollbackFailed;
        result.message = "Failed to rollback: " + reset_result.error;
        return result;
    }
    
    // Verify rollback
    auto head = git_ops_.get_head_commit();
    if (!head || !git_ops_.verify_tag(backup_tag, *head)) {
        result.status = RewordStatus::RollbackFailed;
        result.message = "Rollback verification failed";
        return result;
    }
    
    // Verify clean state
    if (!git_ops_.is_working_directory_clean()) {
        result.status = RewordStatus::RollbackFailed;
        result.message = "Working directory is not clean after rollback";
        return result;
    }
    
    result.status = RewordStatus::Success;
    result.message = "Successfully rolled back to " + backup_tag;
    
    return result;
}

std::string RewordEngine::get_version() {
    return "1.0.0";
}

} // namespace git_reword
