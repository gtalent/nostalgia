from conans import ConanFile, CMake

class NostalgiaConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "jsoncpp/1.9.4", "sdl2/2.0.14@bincrafters/stable"
    generators = "cmake", "cmake_find_package", "cmake_paths"
    #default_options = {
    #    "sdl2:nas": False
    #}
