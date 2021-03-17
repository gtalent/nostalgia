add_library(
	NostalgiaCore-Userspace OBJECT
		gfx.cpp
		gfx_opengl.cpp
		glutils.cpp
		media.cpp
)

if(NOT MSVC)
	target_compile_options(NostalgiaCore-Userspace PRIVATE -Wsign-conversion)
endif()

if(APPLE)
	find_package(OpenGL REQUIRED)
else()
	set(OPENGL_gl_LIBRARY GL)
endif()

target_link_libraries(
	NostalgiaCore-Userspace PUBLIC
		OxClaw
		OxFS
		OxStd
		NostalgiaCore
		${OPENGL_gl_LIBRARY}
)

install(
	TARGETS
		NostalgiaCore-Userspace
	DESTINATION
		include/nostalgia/core
)
