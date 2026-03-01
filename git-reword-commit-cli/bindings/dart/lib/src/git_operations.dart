part of '../git_reword.dart';

/// Git operations wrapper.
class GitOperations {
  Pointer<Void>? _handle;

  /// Create a new GitOperations for the given repository path.
  GitOperations([String repoPath = '.']) {
    final pathPtr = repoPath.toNativeUtf8();
    _handle = _NativeBindings.gitOperationsCreate(pathPtr);
    malloc.free(pathPtr);
  }

  /// Check if git is available.
  bool get isGitAvailable {
    _checkHandle();
    return _NativeBindings.gitOperationsIsGitAvailable(_handle!) != 0;
  }

  /// Check if current directory is a git repository.
  bool get isGitRepository {
    _checkHandle();
    return _NativeBindings.gitOperationsIsGitRepository(_handle!) != 0;
  }

  /// Check if working directory is clean.
  bool get isWorkingDirectoryClean {
    _checkHandle();
    return _NativeBindings.gitOperationsIsWorkingDirectoryClean(_handle!) != 0;
  }

  /// Get the HEAD commit hash.
  String? get headCommit {
    _checkHandle();
    final ptr = _NativeBindings.gitOperationsGetHeadCommit(_handle!);
    if (ptr == nullptr) {
      return null;
    }
    final result = ptr.toDartString();
    _NativeBindings.freeString(ptr);
    return result;
  }

  /// Dispose of the resources.
  void dispose() {
    if (_handle != null) {
      _NativeBindings.gitOperationsDestroy(_handle!);
      _handle = null;
    }
  }

  void _checkHandle() {
    if (_handle == null) {
      throw StateError('GitOperations has been disposed');
    }
  }
}
