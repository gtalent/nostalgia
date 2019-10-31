add_library(
	NostalgiaCore
		core.cpp
)

target_link_libraries(
	NostalgiaCore PUBLIC
		NostalgiaCommon
		OxFS
)

if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	add_subdirectory(gba)
elseif(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_subdirectory(sdl)
	add_subdirectory(userland)
endif()
if(NOSTALGIA_BUILD_STUDIO)
	add_subdirectory(studio)
endif()

install(
	FILES
		consts.hpp
		context.hpp
		core.hpp
		gfx.hpp
		media.hpp
		mem.hpp
		types.hpp
	DESTINATION
		include/nostalgia/core
)
