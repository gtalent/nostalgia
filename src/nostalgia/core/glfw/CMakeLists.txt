add_library(
	NostalgiaCore-GLFW
		core.cpp
		gfx.cpp
)

find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)

if(APPLE)
	find_package(OpenGL REQUIRED)
else()
	set(OPENGL_gl_LIBRARY GL)
endif()

target_link_libraries(
	NostalgiaCore-GLFW PUBLIC
		${OPENGL_gl_LIBRARY}
		glfw::glfw
		imgui::imgui
		imgui-glfw
		NostalgiaGlUtils
		NostalgiaCore-Userspace
)

install(
	TARGETS
		NostalgiaCore-GLFW
	DESTINATION
		LIBRARY DESTINATION lib/nostalgia
		ARCHIVE DESTINATION lib/nostalgia
)
