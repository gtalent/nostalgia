from conans import ConanFile, CMake

class NostalgiaConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "jsoncpp/1.9.2", "sdl2/2.0.10@bincrafters/stable", "qt/5.14.1@bincrafters/stable", "sqlite3/3.31.0", "libiconv/1.16"
    generators = "cmake", "cmake_find_package", "cmake_paths"
    #default_options = {
    #    "sdl2:nas": False
    #}

    def requirements(self):
        pass
