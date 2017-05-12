cmake_minimum_required(VERSION 2.8.11)

if(WOMBAT_BUILD_TYPE STREQUAL "GBA")
	enable_language(C ASM)
	set(
		CPP
			gba/gfx.cpp
			gba/media.cpp
	)
elseif(WOMBAT_BUILD_TYPE STREQUAL "Native")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set(CMAKE_AUTOMOC ON)

	set(
		CPP
			qt/gfx.cpp
	)
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
		gba/gba.hpp
	DESTINATION
		include/nostalgia/core
)
