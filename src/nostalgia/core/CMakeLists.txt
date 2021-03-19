add_library(
	NostalgiaCore
		gfx.cpp
		media.cpp
)

if(NOT MSVC)
	target_compile_options(NostalgiaCore PUBLIC -Wsign-conversion)
endif()

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
		color.hpp
		config.hpp
		consts.hpp
		context.hpp
		core.hpp
		gfx.hpp
		input.hpp
		media.hpp
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
