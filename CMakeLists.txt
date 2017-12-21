cmake_minimum_required(VERSION 2.8.8)

project(nostalgia)

set(NOSTALGIA_BUILD_TYPE "Native" CACHE STRING "The type of build to produce(Native/GBA)")
set(NOSTALGIA_IDE_BUILD "ON" CACHE STRING "Build for IDE's to run")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	include(GBA)
	add_definitions(
		-nostdlib
		-fno-exceptions
		-fno-rtti
		-fno-strict-aliasing
		-mthumb-interwork
		-mthumb
	)
endif()

add_definitions(
	-std=c++14
	#-fdiagnostics-color
	-Wall
	-Wsign-compare
)

if(NOSTALGIA_IDE_BUILD STREQUAL "ON")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if(APPLE)
	set(CMAKE_MACOSX_RPATH OFF)
	set(CMAKE_INSTALL_NAME_DIR "@executable_path/../Library/nostalgia")
	set(NOSTALGIA_DIST_BIN              Nostalgia.app/Contents/MacOS)
	set(NOSTALGIA_DIST_LIB              Nostalgia.app/Contents/Library)
	set(NOSTALGIA_DIST_PLUGIN           Nostalgia.app/Contents/Plugins)
	set(NOSTALGIA_DIST_RESOURCES        Nostalgia.app/Contents/Resources)
	set(NOSTALGIA_DIST_MAC_APP_CONTENTS Nostalgia.app/Contents)
else()
	set(CMAKE_INSTALL_RPATH "$ORIGIN" "$ORIGIN/../lib/nostalgia")
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(NOSTALGIA_DIST_BIN       bin)
	set(NOSTALGIA_DIST_LIB       lib)
	set(NOSTALGIA_DIST_PLUGIN    lib)
	set(NOSTALGIA_DIST_RESOURCES share)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(
		-Werror
	)
endif()

enable_testing()

add_subdirectory(deps/ox)
include_directories(deps/ox/src)

add_subdirectory(src)
