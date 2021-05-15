add_library(
	NostalgiaCore-GLFW
		core.cpp
		gfx.cpp
)

find_package(glfw3 REQUIRED)

target_link_libraries(
	NostalgiaCore-GLFW PUBLIC
		glfw::glfw
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
