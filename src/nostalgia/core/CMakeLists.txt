if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	enable_language(C ASM)
	set(
		CPP
			gba/core.cpp
			gba/gfx.cpp
			gba/media.cpp
			gba/mem.cpp
			gba/panic.cpp
	)
elseif(NOSTALGIA_BUILD_STUDIO)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set(CMAKE_AUTOMOC ON)

	set(
		CPP
			#qt/gfx.cpp
			userland/media.cpp
			userland/mem.cpp
	)
else()
	set(
		CPP
			#qt/gfx.cpp # does not currently use any Qt stuff, but will need to replace with SDL version 
			userland/media.cpp
			userland/mem.cpp
	)
endif()

add_library(
	NostalgiaCore
		${CPP}
		core.cpp
)

target_link_libraries(
	NostalgiaCore PUBLIC
		NostalgiaCommon
		OxFS
)

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_subdirectory(sdl)
endif()
if(NOSTALGIA_BUILD_STUDIO)
	add_subdirectory(studio)
endif()

install(
	FILES
		consts.hpp
		core.hpp
		gfx.hpp
		media.hpp
		gba/gba.hpp
	DESTINATION
		include/nostalgia/core
)
