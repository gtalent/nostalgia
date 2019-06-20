from conans import ConanFile, CMake

class NostalgiaConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "jsoncpp/1.8.4@theirix/stable" # comma-separated list of requirements
    generators = "cmake"
