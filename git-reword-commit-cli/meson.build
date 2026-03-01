project('git-reword-commit-cli', 'cpp',
  version : '1.0.0',
  license : 'MIT',
  meson_version : '>=0.55.0',
  default_options : [
    'cpp_std=c++17',
    'warning_level=3',
    'default_library=static',
    'buildtype=release'
  ]
)

# Compiler detection
cpp = meson.get_compiler('cpp')
cpp_id = cpp.get_id()

# Add compiler-specific flags
if cpp_id == 'msvc'
  add_project_arguments('/utf-8', language: 'cpp')
  add_project_arguments('/D_CRT_SECURE_NO_WARNINGS', language: 'cpp')
elif cpp_id == 'gcc' or cpp_id == 'clang'
  # GCC/Clang flags already handled by warning_level
endif

# Platform-specific dependencies
if host_machine.system() == 'windows'
  shlwapi_dep = cpp.find_library('shlwapi', required: true)
  platform_deps = [shlwapi_dep]
else
  platform_deps = []
endif

# Include directories
inc_dir = include_directories('include')

# Core library sources
core_sources = files(
  'src/platform.cpp',
  'src/utils.cpp',
  'src/git_operations.cpp',
  'src/reword_engine.cpp'
)

# Core library
git_reword_core_lib = library('git_reword_core',
  core_sources,
  include_directories : inc_dir,
  dependencies : platform_deps,
  install : true,
  version : meson.project_version()
)

# Core library dependency for other targets
git_reword_core_dep = declare_dependency(
  link_with : git_reword_core_lib,
  include_directories : inc_dir
)

# Main CLI executable
executable('git-reword-commit',
  'src/main.cpp',
  dependencies : [git_reword_core_dep],
  install : true
)

# Install headers
install_headers(
  'include/git_reword/export.hpp',
  'include/git_reword/platform.hpp',
  'include/git_reword/utils.hpp',
  'include/git_reword/git_operations.hpp',
  'include/git_reword/reword_engine.hpp',
  subdir : 'git_reword'
)

# Package configuration
pkg = import('pkgconfig')
pkg.generate(
  git_reword_core_lib,
  name : 'git-reword-commit-cli',
  description : 'Cross-platform CLI tool for non-interactive git commit rewording',
  url : 'https://github.com/Baneeishaque/AI-Agents'
)

# Summary
summary({
  'Compiler': cpp_id,
  'Build type': get_option('buildtype'),
  'Platform': host_machine.system(),
}, section: 'Configuration')
