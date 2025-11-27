part of '../git_reword.dart';

/// Native bindings for git_reword_dart library.
class _NativeBindings {
  static final DynamicLibrary _lib = _loadLibrary();

  static DynamicLibrary _loadLibrary() {
    if (Platform.isWindows) {
      return DynamicLibrary.open('git_reword_dart.dll');
    } else if (Platform.isMacOS) {
      return DynamicLibrary.open('libgit_reword_dart.dylib');
    } else if (Platform.isLinux || Platform.isAndroid) {
      return DynamicLibrary.open('libgit_reword_dart.so');
    } else {
      throw UnsupportedError('Unsupported platform: ${Platform.operatingSystem}');
    }
  }

  // Platform functions
  static final int Function() platformIsWindows =
      _lib.lookupFunction<Int32 Function(), int Function()>('platform_is_windows');

  static final int Function() platformIsMacOS =
      _lib.lookupFunction<Int32 Function(), int Function()>('platform_is_macos');

  static final int Function() platformIsLinux =
      _lib.lookupFunction<Int32 Function(), int Function()>('platform_is_linux');

  static final int Function() platformIsAndroid =
      _lib.lookupFunction<Int32 Function(), int Function()>('platform_is_android');

  static final int Function() platformIsIOS =
      _lib.lookupFunction<Int32 Function(), int Function()>('platform_is_ios');

  static final int Function() platformIsTermux =
      _lib.lookupFunction<Int32 Function(), int Function()>('platform_is_termux');

  static final Pointer<Utf8> Function() platformGetName =
      _lib.lookupFunction<Pointer<Utf8> Function(), Pointer<Utf8> Function()>('platform_get_name');

  // Memory management
  static final void Function(Pointer<Utf8>) freeString =
      _lib.lookupFunction<Void Function(Pointer<Utf8>), void Function(Pointer<Utf8>)>('free_string');

  static final void Function(Pointer<_FFIRewordResult>) freeRewordResult = _lib.lookupFunction<
      Void Function(Pointer<_FFIRewordResult>),
      void Function(Pointer<_FFIRewordResult>)>('free_reword_result');

  // RewordEngine functions
  static final Pointer<Void> Function(Pointer<Utf8>) rewordEngineCreate = _lib.lookupFunction<
      Pointer<Void> Function(Pointer<Utf8>),
      Pointer<Void> Function(Pointer<Utf8>)>('reword_engine_create');

  static final void Function(Pointer<Void>) rewordEngineDestroy =
      _lib.lookupFunction<Void Function(Pointer<Void>), void Function(Pointer<Void>)>('reword_engine_destroy');

  static final Pointer<Utf8> Function() rewordEngineGetVersion =
      _lib.lookupFunction<Pointer<Utf8> Function(), Pointer<Utf8> Function()>('reword_engine_get_version');

  static final Pointer<_FFIRewordResult> Function(Pointer<Void>) rewordEnginePreflightChecks =
      _lib.lookupFunction<
          Pointer<_FFIRewordResult> Function(Pointer<Void>),
          Pointer<_FFIRewordResult> Function(Pointer<Void>)>('reword_engine_preflight_checks');

  static final Pointer<_FFIRewordResult> Function(Pointer<Void>, Pointer<Utf8>, Pointer<Utf8>, int) 
      rewordEngineRewordCommit = _lib.lookupFunction<
          Pointer<_FFIRewordResult> Function(Pointer<Void>, Pointer<Utf8>, Pointer<Utf8>, Int32),
          Pointer<_FFIRewordResult> Function(Pointer<Void>, Pointer<Utf8>, Pointer<Utf8>, int)>('reword_engine_reword_commit');

  static final Pointer<_FFIRewordResult> Function(Pointer<Void>, Pointer<Utf8>) rewordEngineRollback =
      _lib.lookupFunction<
          Pointer<_FFIRewordResult> Function(Pointer<Void>, Pointer<Utf8>),
          Pointer<_FFIRewordResult> Function(Pointer<Void>, Pointer<Utf8>)>('reword_engine_rollback');

  // GitOperations functions
  static final Pointer<Void> Function(Pointer<Utf8>) gitOperationsCreate = _lib.lookupFunction<
      Pointer<Void> Function(Pointer<Utf8>),
      Pointer<Void> Function(Pointer<Utf8>)>('git_operations_create');

  static final void Function(Pointer<Void>) gitOperationsDestroy =
      _lib.lookupFunction<Void Function(Pointer<Void>), void Function(Pointer<Void>)>('git_operations_destroy');

  static final int Function(Pointer<Void>) gitOperationsIsGitAvailable =
      _lib.lookupFunction<Int32 Function(Pointer<Void>), int Function(Pointer<Void>)>('git_operations_is_git_available');

  static final int Function(Pointer<Void>) gitOperationsIsGitRepository =
      _lib.lookupFunction<Int32 Function(Pointer<Void>), int Function(Pointer<Void>)>('git_operations_is_git_repository');

  static final int Function(Pointer<Void>) gitOperationsIsWorkingDirectoryClean = _lib.lookupFunction<
      Int32 Function(Pointer<Void>),
      int Function(Pointer<Void>)>('git_operations_is_working_directory_clean');

  static final Pointer<Utf8> Function(Pointer<Void>) gitOperationsGetHeadCommit = _lib.lookupFunction<
      Pointer<Utf8> Function(Pointer<Void>),
      Pointer<Utf8> Function(Pointer<Void>)>('git_operations_get_head_commit');
}

/// FFI struct for RewordResult
final class _FFIRewordResult extends Struct {
  @Int32()
  external int status;

  external Pointer<Utf8> message;
  external Pointer<Utf8> oldCommitHash;
  external Pointer<Utf8> newCommitHash;
  external Pointer<Utf8> backupTag;
}
