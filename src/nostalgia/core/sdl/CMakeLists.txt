add_library(
	NostalgiaCore-SDL
		core.cpp
		gfx.cpp
)

find_package(sdl2 REQUIRED)

target_link_libraries(
	NostalgiaCore-SDL PUBLIC
		sdl2::sdl2
		OxClaw
		OxFS
		OxStd
		NostalgiaCore-Userspace
)

install(
	TARGETS
		NostalgiaCore-SDL
	DESTINATION
		include/nostalgia/core
)
