from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy
import os


class GitRewordCommitCliConan(ConanFile):
    name = "git-reword-commit-cli"
    version = "1.0.0"
    license = "MIT"
    author = "Baneeishaque K"
    url = "https://github.com/Baneeishaque/AI-Agents"
    description = "Cross-platform CLI tool for non-interactive git commit rewording"
    topics = ("git", "commit", "reword", "cli", "cross-platform")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_python_bindings": [True, False],
        "build_java_bindings": [True, False],
        "build_dart_bindings": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_python_bindings": False,
        "build_java_bindings": False,
        "build_dart_bindings": False,
    }
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "cmake/*", "bindings/*"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["BUILD_SHARED_LIBS"] = self.options.shared
        tc.variables["BUILD_PYTHON_BINDINGS"] = self.options.build_python_bindings
        tc.variables["BUILD_JAVA_BINDINGS"] = self.options.build_java_bindings
        tc.variables["BUILD_DART_BINDINGS"] = self.options.build_dart_bindings
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "LICENSE", src=os.path.join(self.source_folder, ".."), 
             dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.libs = ["git_reword_core"]
        self.cpp_info.set_property("cmake_target_name", "GitReword::git_reword_core")
        
        # Platform-specific libraries
        if self.settings.os == "Windows":
            self.cpp_info.system_libs = ["shlwapi"]
