add_library(
	NostalgiaCore-Userspace OBJECT
		gfx.cpp
		gfx_opengl.cpp
		media.cpp
)

if(NOT MSVC)
	target_compile_options(NostalgiaCore-Userspace PRIVATE -Wsign-conversion)
endif()

target_link_libraries(
	NostalgiaCore-Userspace PUBLIC
		OxClaw
		OxFS
		OxStd
		NostalgiaCore
		NostalgiaGlUtils
)

install(
	TARGETS
		NostalgiaCore-Userspace
	DESTINATION
		include/nostalgia/core
)
