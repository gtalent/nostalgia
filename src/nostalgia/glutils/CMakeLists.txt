add_library(
	NostalgiaGlUtils OBJECT
		glutils.cpp
)

if(NOT MSVC)
	target_compile_options(NostalgiaGlUtils PRIVATE -Wsign-conversion)
endif()

if(APPLE)
	find_package(OpenGL REQUIRED)
else()
	set(OPENGL_gl_LIBRARY GL)
endif()

target_link_libraries(
	NostalgiaGlUtils PUBLIC
		OxStd
		${OPENGL_gl_LIBRARY}
)

install(
	TARGETS
		NostalgiaGlUtils
	DESTINATION
		include/nostalgia/glutils
)
