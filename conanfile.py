from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.scm import Git


class libtusclientRecipe(ConanFile):
    name = "libtusclient"
    version = "0.1"
    package_type = "library"

    # Optional metadata
    license = "MIT"
    author = "Cadons"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "https://github.com/Cadons/libtusclient"
    topics = ("tus protocol", "tus client", "tus upload", "tus resumable upload", "tus.io")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "test/*", "config.h"

    def get_version(self):
        git = Git(self)
        tag=git.run("tag")
        branch=git.run("branch --show-current")
        if tag:
            return tag
        return branch+"-"+ self.version
    
    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.cache_variables["CONAN_NAME"] = self.name
        tc.cache_variables["CONAN_VERSION"] = self.get_version()
        tc.cache_variables["CONAN_PACKAGE_LICENCE"] = self.license
        

        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["libtusclient"]
    
    def requirements(self):
        self.requires("libcurl/8.9.1")
        self.requires("boost/1.86.0")
        self.requires("libzippp/7.1-1.10.1")
        self.requires("gtest/1.15.0")

