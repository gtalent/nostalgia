cmake_minimum_required(VERSION 2.8.8)

if(WOMBAT_BUILD_TYPE STREQUAL "GBA")
	set(
		CPP
			gba/core.cpp
			gba/gfx.cpp
	)
	include_directories("gba")
elseif(WOMBAT_BUILD_TYPE STREQUAL "Native")
endif()

add_library(NostalgiaCore OBJECT ${CPP})

install(
	FILES
	DESTINATION
		include/nostalgia/core
)
