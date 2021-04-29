from conans import ConanFile, CMake

class NostalgiaConan(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    requires = 'jsoncpp/1.9.4', 'glfw/3.3.4'
    generators = 'cmake', 'cmake_find_package', 'cmake_paths'
    default_options = {
    }
