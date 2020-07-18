add_library(
	NostalgiaCore
		gfx.cpp
		media.cpp
)

target_compile_options(NostalgiaCore PUBLIC -Wsign-conversion)

target_link_libraries(
	NostalgiaCore PUBLIC
		OxFS
)

add_subdirectory(gba)
if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_subdirectory(sdl)
	add_subdirectory(userland)
endif()
if(NOSTALGIA_BUILD_STUDIO)
	add_subdirectory(qt)
	add_subdirectory(studio)
endif()

install(
	FILES
		config.hpp
		consts.hpp
		context.hpp
		core.hpp
		gfx.hpp
		input.hpp
		media.hpp
		mem.hpp
	DESTINATION
		include/nostalgia/core
)

install(
	TARGETS
		NostalgiaCore
	DESTINATION
		LIBRARY DESTINATION lib/nostalgia
		ARCHIVE DESTINATION lib/nostalgia
)
