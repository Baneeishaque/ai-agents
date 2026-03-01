/**
 * @file main.cpp
 * @brief CLI entry point for git-reword-commit
 */

#include "git_reword/reword_engine.hpp"
#include "git_reword/platform.hpp"
#include "git_reword/utils.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace {

void print_version() {
    std::cout << "git-reword-commit v" << git_reword::RewordEngine::get_version() << std::endl;
    std::cout << "Platform: " << git_reword::platform::get_platform_name() << std::endl;
}

void print_usage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS] <commit-hash> <new-message>\n"
              << "\n"
              << "Reword a git commit non-interactively.\n"
              << "\n"
              << "Arguments:\n"
              << "  <commit-hash>    The hash of the commit to reword\n"
              << "  <new-message>    The new commit message\n"
              << "\n"
              << "Options:\n"
              << "  -h, --help       Show this help message\n"
              << "  -v, --version    Show version information\n"
              << "  -C <path>        Run as if started in <path>\n"
              << "  --keep-backup    Keep the backup tag after successful reword\n"
              << "  --check-only     Only perform pre-flight checks\n"
              << "  --verbose        Show detailed progress output\n"
              << "  --rollback <tag> Rollback to a backup tag\n"
              << "\n"
              << "Examples:\n"
              << "  " << program_name << " abc1234 \"Fix: correct typo in README\"\n"
              << "  " << program_name << " -C /path/to/repo HEAD~2 \"Refactor: improve performance\"\n"
              << "  " << program_name << " --rollback backup-before-reword-1234567890\n"
              << std::endl;
}

bool parse_arguments(
    int argc, char* argv[],
    std::string& repo_path,
    std::string& commit_hash,
    std::string& new_message,
    std::string& rollback_tag,
    bool& keep_backup,
    bool& check_only,
    bool& verbose,
    bool& show_help,
    bool& show_version) {
    
    repo_path = ".";
    keep_backup = false;
    check_only = false;
    verbose = false;
    show_help = false;
    show_version = false;
    
    std::vector<std::string> positional;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            show_help = true;
            return true;
        } else if (arg == "-v" || arg == "--version") {
            show_version = true;
            return true;
        } else if (arg == "-C" && i + 1 < argc) {
            repo_path = argv[++i];
        } else if (arg == "--keep-backup") {
            keep_backup = true;
        } else if (arg == "--check-only") {
            check_only = true;
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--rollback" && i + 1 < argc) {
            rollback_tag = argv[++i];
        } else if (arg[0] != '-') {
            positional.push_back(arg);
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return false;
        }
    }
    
    // If rollback is specified, we don't need positional args
    if (!rollback_tag.empty()) {
        return true;
    }
    
    // If check-only, we don't need positional args
    if (check_only) {
        return true;
    }
    
    // Otherwise, we need exactly 2 positional arguments
    if (positional.size() != 2) {
        if (positional.empty()) {
            // No arguments provided, show help
            show_help = true;
            return true;
        }
        std::cerr << "Error: Expected 2 arguments (commit-hash and new-message), got " 
                  << positional.size() << std::endl;
        return false;
    }
    
    commit_hash = positional[0];
    new_message = positional[1];
    
    return true;
}

} // anonymous namespace

int main(int argc, char* argv[]) {
    std::string repo_path;
    std::string commit_hash;
    std::string new_message;
    std::string rollback_tag;
    bool keep_backup;
    bool check_only;
    bool verbose;
    bool show_help;
    bool show_version;
    
    if (!parse_arguments(argc, argv, repo_path, commit_hash, new_message, 
                        rollback_tag, keep_backup, check_only, verbose, 
                        show_help, show_version)) {
        std::cerr << "Use --help for usage information." << std::endl;
        return 1;
    }
    
    if (show_version) {
        print_version();
        return 0;
    }
    
    if (show_help) {
        print_usage(argv[0]);
        return 0;
    }
    
    // Create the reword engine
    git_reword::RewordEngine engine(repo_path);
    
    // Set up progress callback if verbose
    if (verbose) {
        engine.set_progress_callback([](const std::string& step, const std::string& details) {
            std::cout << ">> " << step;
            if (!details.empty()) {
                std::cout << ": " << details;
            }
            std::cout << std::endl;
        });
    }
    
    // Handle rollback
    if (!rollback_tag.empty()) {
        std::cout << "Rolling back to " << rollback_tag << "..." << std::endl;
        auto result = engine.rollback(rollback_tag);
        
        if (result.status != git_reword::RewordStatus::Success) {
            std::cerr << "Rollback failed: " << result.message << std::endl;
            return 1;
        }
        
        std::cout << "Rollback successful!" << std::endl;
        return 0;
    }
    
    // Handle check-only
    if (check_only) {
        std::cout << "Performing pre-flight checks..." << std::endl;
        auto result = engine.preflight_checks();
        
        if (result.status != git_reword::RewordStatus::Success) {
            std::cerr << "Pre-flight check failed: " << result.message << std::endl;
            return 1;
        }
        
        std::cout << "All pre-flight checks passed!" << std::endl;
        if (!result.message.empty()) {
            std::cout << result.message << std::endl;
        }
        return 0;
    }
    
    // Perform the reword operation
    std::cout << "Rewording commit " << commit_hash << "..." << std::endl;
    auto result = engine.reword_commit(commit_hash, new_message, keep_backup);
    
    if (result.status != git_reword::RewordStatus::Success) {
        std::cerr << "Error: " << result.message << std::endl;
        return 1;
    }
    
    std::cout << "Successfully reworded commit!" << std::endl;
    std::cout << "Old commit: " << result.old_commit_hash << std::endl;
    std::cout << "New commit: " << result.new_commit_hash << std::endl;
    
    if (!result.backup_tag.empty()) {
        std::cout << "Backup tag: " << result.backup_tag << " (use --rollback to restore)" << std::endl;
    }
    
    return 0;
}
