cmake_minimum_required(VERSION 2.8.8)

if(WOMBAT_BUILD_TYPE STREQUAL "GBA")
	enable_language(C ASM)
	set(
		CPP
			gba/gfx.cpp
			gba/media.cpp
			gba/dirt.s
	)
elseif(WOMBAT_BUILD_TYPE STREQUAL "Native")
endif()

add_library(
	NostalgiaCore
		${CPP}
		core.cpp
)

install(
	FILES
		core.hpp
		gfx.hpp
	DESTINATION
		include/nostalgia/core
)
