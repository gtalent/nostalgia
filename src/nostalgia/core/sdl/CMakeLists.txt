add_library(
	NostalgiaCore-SDL
		core.cpp
		gfx.cpp
)

target_link_libraries(
	NostalgiaCore-SDL PUBLIC
		SDL2::Main
		OxFS
		OxStd
)

install(
	TARGETS
		NostalgiaCore-SDL
	DESTINATION
		${NOSTALGIA_DIST_PLUGIN}
)
