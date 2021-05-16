from conans import ConanFile, CMake

class NostalgiaConan(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    requires = 'jsoncpp/1.9.4', 'glfw/3.3.4', 'imgui/1.82'
    generators = 'cmake', 'cmake_find_package', 'cmake_paths'
    default_options = {
    }

    def imports(self):
        self.copy('imgui_impl_glfw.cpp',    dst='../deps/imgui/src', src='./res/bindings')
        self.copy('imgui_impl_opengl3.cpp', dst='../deps/imgui/src', src='./res/bindings')
        self.copy('imgui_impl_glfw.h',      dst='../deps/imgui/src', src='./res/bindings')
        self.copy('imgui_impl_opengl3.h',   dst='../deps/imgui/src', src='./res/bindings')
