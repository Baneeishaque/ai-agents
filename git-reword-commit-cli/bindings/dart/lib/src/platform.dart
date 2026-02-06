part of '../git_reword.dart';

/// Platform detection utilities.
class GitRewordPlatform {
  GitRewordPlatform._();

  /// Check if running on Windows.
  static bool get isWindows => _NativeBindings.platformIsWindows() != 0;

  /// Check if running on macOS.
  static bool get isMacOS => _NativeBindings.platformIsMacOS() != 0;

  /// Check if running on Linux.
  static bool get isLinux => _NativeBindings.platformIsLinux() != 0;

  /// Check if running on Android.
  static bool get isAndroid => _NativeBindings.platformIsAndroid() != 0;

  /// Check if running on iOS.
  static bool get isIOS => _NativeBindings.platformIsIOS() != 0;

  /// Check if running in Termux environment.
  static bool get isTermux => _NativeBindings.platformIsTermux() != 0;

  /// Get the platform name.
  static String get name {
    final ptr = _NativeBindings.platformGetName();
    final result = ptr.toDartString();
    _NativeBindings.freeString(ptr);
    return result;
  }
}
