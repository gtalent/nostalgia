add_library(
	NostalgiaCore-SDL
		core.cpp
		gfx.cpp
)

find_package(sdl2 REQUIRED)

target_link_libraries(
	NostalgiaCore-SDL PUBLIC
		SDL2::SDL2
		NostalgiaCore-Userspace
)

install(
	TARGETS
		NostalgiaCore-SDL
	DESTINATION
		LIBRARY DESTINATION lib/nostalgia
		ARCHIVE DESTINATION lib/nostalgia
)
