add_library(
	imgui-glfw
		src/imgui_impl_glfw.cpp
		src/imgui_impl_opengl3.cpp
)

find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)

if(APPLE)
	target_compile_definitions(
		imgui-glfw PRIVATE
			IMGUI_IMPL_OPENGL_LOADER_CUSTOM="OpenGL/gl3.h"
	)
endif()

target_link_libraries(
	imgui-glfw PUBLIC
		${OPENGL_gl_LIBRARY}
		glfw::glfw
		imgui::imgui
)

if(NOT MSVC)
	target_compile_options(
		imgui-glfw PRIVATE
			-Wno-all
			-Wno-extra
			-Wno-old-style-cast
			-Wno-deprecated-declarations
	)
endif()

install(
	TARGETS
		imgui-glfw
	DESTINATION
		LIBRARY DESTINATION lib/nostalgia
		ARCHIVE DESTINATION lib/nostalgia
)
