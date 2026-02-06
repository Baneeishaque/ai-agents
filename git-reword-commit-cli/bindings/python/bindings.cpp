/**
 * @file bindings.cpp
 * @brief Python bindings for git_reword library using pybind11
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "git_reword/git_operations.hpp"
#include "git_reword/reword_engine.hpp"
#include "git_reword/platform.hpp"
#include "git_reword/utils.hpp"

namespace py = pybind11;

PYBIND11_MODULE(git_reword, m) {
    m.doc() = "Python bindings for git-reword-commit-cli";

    // Platform module
    auto platform_module = m.def_submodule("platform", "Platform detection utilities");
    platform_module.def("is_windows", &git_reword::platform::is_windows);
    platform_module.def("is_macos", &git_reword::platform::is_macos);
    platform_module.def("is_linux", &git_reword::platform::is_linux);
    platform_module.def("is_android", &git_reword::platform::is_android);
    platform_module.def("is_ios", &git_reword::platform::is_ios);
    platform_module.def("is_termux", &git_reword::platform::is_termux);
    platform_module.def("get_platform_name", &git_reword::platform::get_platform_name);

    // Utils module
    auto utils_module = m.def_submodule("utils", "Utility functions");
    utils_module.def("file_exists", &git_reword::utils::file_exists);
    utils_module.def("directory_exists", &git_reword::utils::directory_exists);
    utils_module.def("get_timestamp", &git_reword::utils::get_timestamp);
    utils_module.def("generate_backup_tag_name", &git_reword::utils::generate_backup_tag_name);

    // GitResult struct
    py::class_<git_reword::GitResult>(m, "GitResult")
        .def(py::init<>())
        .def_readwrite("success", &git_reword::GitResult::success)
        .def_readwrite("output", &git_reword::GitResult::output)
        .def_readwrite("error", &git_reword::GitResult::error)
        .def_readwrite("exit_code", &git_reword::GitResult::exit_code);

    // CommitInfo struct
    py::class_<git_reword::CommitInfo>(m, "CommitInfo")
        .def(py::init<>())
        .def_readwrite("hash", &git_reword::CommitInfo::hash)
        .def_readwrite("short_hash", &git_reword::CommitInfo::short_hash)
        .def_readwrite("message", &git_reword::CommitInfo::message)
        .def_readwrite("author", &git_reword::CommitInfo::author)
        .def_readwrite("date", &git_reword::CommitInfo::date);

    // RewordStatus enum
    py::enum_<git_reword::RewordStatus>(m, "RewordStatus")
        .value("Success", git_reword::RewordStatus::Success)
        .value("GitNotAvailable", git_reword::RewordStatus::GitNotAvailable)
        .value("GitLfsNotAvailable", git_reword::RewordStatus::GitLfsNotAvailable)
        .value("NotAGitRepository", git_reword::RewordStatus::NotAGitRepository)
        .value("WorkingDirectoryDirty", git_reword::RewordStatus::WorkingDirectoryDirty)
        .value("RebaseInProgress", git_reword::RewordStatus::RebaseInProgress)
        .value("CommitNotFound", git_reword::RewordStatus::CommitNotFound)
        .value("RebaseFailed", git_reword::RewordStatus::RebaseFailed)
        .value("AmendFailed", git_reword::RewordStatus::AmendFailed)
        .value("RollbackFailed", git_reword::RewordStatus::RollbackFailed)
        .value("UnknownError", git_reword::RewordStatus::UnknownError)
        .export_values();

    m.def("status_to_string", &git_reword::status_to_string);

    // RewordResult struct
    py::class_<git_reword::RewordResult>(m, "RewordResult")
        .def(py::init<>())
        .def_readwrite("status", &git_reword::RewordResult::status)
        .def_readwrite("message", &git_reword::RewordResult::message)
        .def_readwrite("old_commit_hash", &git_reword::RewordResult::old_commit_hash)
        .def_readwrite("new_commit_hash", &git_reword::RewordResult::new_commit_hash)
        .def_readwrite("backup_tag", &git_reword::RewordResult::backup_tag);

    // GitOperations class
    py::class_<git_reword::GitOperations>(m, "GitOperations")
        .def(py::init<const std::string&>(), py::arg("repo_path") = ".")
        .def("is_git_available", &git_reword::GitOperations::is_git_available)
        .def("is_git_lfs_available", &git_reword::GitOperations::is_git_lfs_available)
        .def("is_git_repository", &git_reword::GitOperations::is_git_repository)
        .def("is_working_directory_clean", &git_reword::GitOperations::is_working_directory_clean)
        .def("is_rebase_in_progress", &git_reword::GitOperations::is_rebase_in_progress)
        .def("get_head_commit", &git_reword::GitOperations::get_head_commit)
        .def("get_commit_info", &git_reword::GitOperations::get_commit_info)
        .def("create_tag", &git_reword::GitOperations::create_tag)
        .def("delete_tag", &git_reword::GitOperations::delete_tag)
        .def("get_log", &git_reword::GitOperations::get_log, py::arg("count") = 5)
        .def("get_repo_path", &git_reword::GitOperations::get_repo_path);

    // RewordEngine class
    py::class_<git_reword::RewordEngine>(m, "RewordEngine")
        .def(py::init<const std::string&>(), py::arg("repo_path") = ".")
        .def("set_progress_callback", &git_reword::RewordEngine::set_progress_callback)
        .def("preflight_checks", &git_reword::RewordEngine::preflight_checks)
        .def("reword_commit", &git_reword::RewordEngine::reword_commit,
             py::arg("commit_hash"),
             py::arg("new_message"),
             py::arg("keep_backup") = false)
        .def("rollback", &git_reword::RewordEngine::rollback)
        .def_static("get_version", &git_reword::RewordEngine::get_version);

    // Module version
    m.attr("__version__") = "1.0.0";
}
