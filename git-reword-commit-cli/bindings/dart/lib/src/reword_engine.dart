part of '../git_reword.dart';

/// Status codes for reword operations.
enum RewordStatus {
  success,
  gitNotAvailable,
  gitLfsNotAvailable,
  notAGitRepository,
  workingDirectoryDirty,
  rebaseInProgress,
  commitNotFound,
  rebaseFailed,
  amendFailed,
  rollbackFailed,
  unknownError;

  static RewordStatus fromValue(int value) {
    if (value >= 0 && value < RewordStatus.values.length) {
      return RewordStatus.values[value];
    }
    return RewordStatus.unknownError;
  }
}

/// Result of a reword operation.
class RewordResult {
  final RewordStatus status;
  final String message;
  final String oldCommitHash;
  final String newCommitHash;
  final String backupTag;

  RewordResult({
    required this.status,
    required this.message,
    required this.oldCommitHash,
    required this.newCommitHash,
    required this.backupTag,
  });

  bool get isSuccess => status == RewordStatus.success;

  factory RewordResult._fromFFI(Pointer<_FFIRewordResult> ptr) {
    final result = RewordResult(
      status: RewordStatus.fromValue(ptr.ref.status),
      message: ptr.ref.message.toDartString(),
      oldCommitHash: ptr.ref.oldCommitHash.toDartString(),
      newCommitHash: ptr.ref.newCommitHash.toDartString(),
      backupTag: ptr.ref.backupTag.toDartString(),
    );
    _NativeBindings.freeRewordResult(ptr);
    return result;
  }

  @override
  String toString() =>
      'RewordResult(status: $status, message: $message, oldCommitHash: $oldCommitHash, newCommitHash: $newCommitHash, backupTag: $backupTag)';
}

/// Main engine for rewording git commits.
class RewordEngine {
  Pointer<Void>? _handle;

  /// Create a new RewordEngine for the given repository path.
  RewordEngine([String repoPath = '.']) {
    final pathPtr = repoPath.toNativeUtf8();
    _handle = _NativeBindings.rewordEngineCreate(pathPtr);
    malloc.free(pathPtr);
  }

  /// Get the library version.
  static String get version {
    final ptr = _NativeBindings.rewordEngineGetVersion();
    final result = ptr.toDartString();
    _NativeBindings.freeString(ptr);
    return result;
  }

  /// Perform pre-flight checks.
  RewordResult preflightChecks() {
    _checkHandle();
    final ptr = _NativeBindings.rewordEnginePreflightChecks(_handle!);
    return RewordResult._fromFFI(ptr);
  }

  /// Reword a commit with a new message.
  RewordResult rewordCommit(
    String commitHash,
    String newMessage, {
    bool keepBackup = false,
  }) {
    _checkHandle();
    final commitHashPtr = commitHash.toNativeUtf8();
    final newMessagePtr = newMessage.toNativeUtf8();
    final ptr = _NativeBindings.rewordEngineRewordCommit(
      _handle!,
      commitHashPtr,
      newMessagePtr,
      keepBackup ? 1 : 0,
    );
    malloc.free(commitHashPtr);
    malloc.free(newMessagePtr);
    return RewordResult._fromFFI(ptr);
  }

  /// Rollback to a backup tag.
  RewordResult rollback(String backupTag) {
    _checkHandle();
    final backupTagPtr = backupTag.toNativeUtf8();
    final ptr = _NativeBindings.rewordEngineRollback(_handle!, backupTagPtr);
    malloc.free(backupTagPtr);
    return RewordResult._fromFFI(ptr);
  }

  /// Dispose of the engine resources.
  void dispose() {
    if (_handle != null) {
      _NativeBindings.rewordEngineDestroy(_handle!);
      _handle = null;
    }
  }

  void _checkHandle() {
    if (_handle == null) {
      throw StateError('RewordEngine has been disposed');
    }
  }
}
