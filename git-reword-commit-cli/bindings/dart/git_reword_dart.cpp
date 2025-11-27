/**
 * @file git_reword_dart.cpp
 * @brief Dart FFI bindings for git_reword library
 */

#include <cstring>
#include <cstdlib>
#include "git_reword/reword_engine.hpp"
#include "git_reword/git_operations.hpp"
#include "git_reword/platform.hpp"

// Dart FFI requires C linkage
extern "C" {

// Helper to allocate and copy string (caller must free)
char* copy_string(const std::string& str) {
    char* result = static_cast<char*>(malloc(str.length() + 1));
    if (result) {
        strcpy(result, str.c_str());
    }
    return result;
}

// Memory management
void free_string(char* str) {
    free(str);
}

// Platform functions
int platform_is_windows() {
    return git_reword::platform::is_windows() ? 1 : 0;
}

int platform_is_macos() {
    return git_reword::platform::is_macos() ? 1 : 0;
}

int platform_is_linux() {
    return git_reword::platform::is_linux() ? 1 : 0;
}

int platform_is_android() {
    return git_reword::platform::is_android() ? 1 : 0;
}

int platform_is_ios() {
    return git_reword::platform::is_ios() ? 1 : 0;
}

int platform_is_termux() {
    return git_reword::platform::is_termux() ? 1 : 0;
}

char* platform_get_name() {
    return copy_string(git_reword::platform::get_platform_name());
}

// RewordResult struct for FFI
struct FFIRewordResult {
    int status;
    char* message;
    char* old_commit_hash;
    char* new_commit_hash;
    char* backup_tag;
};

void free_reword_result(FFIRewordResult* result) {
    if (result) {
        free_string(result->message);
        free_string(result->old_commit_hash);
        free_string(result->new_commit_hash);
        free_string(result->backup_tag);
        free(result);
    }
}

FFIRewordResult* create_ffi_result(const git_reword::RewordResult& result) {
    auto* ffi_result = static_cast<FFIRewordResult*>(malloc(sizeof(FFIRewordResult)));
    if (ffi_result) {
        ffi_result->status = static_cast<int>(result.status);
        ffi_result->message = copy_string(result.message);
        ffi_result->old_commit_hash = copy_string(result.old_commit_hash);
        ffi_result->new_commit_hash = copy_string(result.new_commit_hash);
        ffi_result->backup_tag = copy_string(result.backup_tag);
    }
    return ffi_result;
}

// RewordEngine functions
void* reword_engine_create(const char* repo_path) {
    return new git_reword::RewordEngine(repo_path ? repo_path : ".");
}

void reword_engine_destroy(void* handle) {
    delete static_cast<git_reword::RewordEngine*>(handle);
}

char* reword_engine_get_version() {
    return copy_string(git_reword::RewordEngine::get_version());
}

FFIRewordResult* reword_engine_preflight_checks(void* handle) {
    auto* engine = static_cast<git_reword::RewordEngine*>(handle);
    return create_ffi_result(engine->preflight_checks());
}

FFIRewordResult* reword_engine_reword_commit(void* handle, const char* commit_hash, 
                                              const char* new_message, int keep_backup) {
    auto* engine = static_cast<git_reword::RewordEngine*>(handle);
    return create_ffi_result(engine->reword_commit(commit_hash, new_message, keep_backup != 0));
}

FFIRewordResult* reword_engine_rollback(void* handle, const char* backup_tag) {
    auto* engine = static_cast<git_reword::RewordEngine*>(handle);
    return create_ffi_result(engine->rollback(backup_tag));
}

// GitOperations functions
void* git_operations_create(const char* repo_path) {
    return new git_reword::GitOperations(repo_path ? repo_path : ".");
}

void git_operations_destroy(void* handle) {
    delete static_cast<git_reword::GitOperations*>(handle);
}

int git_operations_is_git_available(void* handle) {
    auto* ops = static_cast<git_reword::GitOperations*>(handle);
    return ops->is_git_available() ? 1 : 0;
}

int git_operations_is_git_repository(void* handle) {
    auto* ops = static_cast<git_reword::GitOperations*>(handle);
    return ops->is_git_repository() ? 1 : 0;
}

int git_operations_is_working_directory_clean(void* handle) {
    auto* ops = static_cast<git_reword::GitOperations*>(handle);
    return ops->is_working_directory_clean() ? 1 : 0;
}

char* git_operations_get_head_commit(void* handle) {
    auto* ops = static_cast<git_reword::GitOperations*>(handle);
    auto head = ops->get_head_commit();
    if (head) {
        return copy_string(*head);
    }
    return nullptr;
}

} // extern "C"
