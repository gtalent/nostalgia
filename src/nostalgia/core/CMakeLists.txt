cmake_minimum_required(VERSION 2.8.11)

if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	enable_language(C ASM)
	set(
		CPP
			gba/gfx.cpp
			gba/media.cpp
			gba/mem.cpp
			gba/panic.cpp
	)
elseif(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	set(CMAKE_AUTOMOC ON)

	set(
		CPP
			qt/gfx.cpp
			userland/mem.cpp
	)
endif()

add_library(
	NostalgiaCore
		${CPP}
		core.cpp
)

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_subdirectory(studio)
endif()

install(
	FILES
		core.hpp
		gfx.hpp
		gba/gba.hpp
	DESTINATION
		include/nostalgia/core
)
