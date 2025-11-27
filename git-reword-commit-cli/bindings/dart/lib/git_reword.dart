/// Dart FFI bindings for git-reword-commit-cli.
/// 
/// A cross-platform library for non-interactive git commit rewording.
library git_reword;

import 'dart:ffi';
import 'dart:io' show Platform;

import 'package:ffi/ffi.dart';

part 'src/bindings.dart';
part 'src/platform.dart';
part 'src/reword_engine.dart';
part 'src/git_operations.dart';
