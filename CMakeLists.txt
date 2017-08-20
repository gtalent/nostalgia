cmake_minimum_required(VERSION 2.8.8)

project(nostalgia)

set(WOMBAT_BUILD_TYPE "Native" CACHE STRING "The type of build to produce(Native/GBA)")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

if (WOMBAT_BUILD_TYPE STREQUAL "GBA")
	include(GBA)
	add_definitions(
		-nostdlib
		-fno-exceptions
		-fno-rtti
	)
endif()

add_definitions(
	-std=c++11
	-Wall
	-Wsign-compare
)

set(CMAKE_INSTALL_RPATH "$ORIGIN" "$ORIGIN/../lib/nostalgia")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(
		-Werror
	)
endif()

enable_testing()

add_subdirectory(deps/ox)
include_directories(deps/ox/src)

add_subdirectory(src)
