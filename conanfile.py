from conans import ConanFile, CMake

class NostalgiaConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "jsoncpp/1.9.0@theirix/stable", "sdl2/2.0.10@bincrafters/stable"#, "qt/5.14.0@bincrafters/stable" # comma-separated list of requirements
    generators = "cmake", "cmake_find_package", "cmake_paths"

    def requirements(self):
        pass
