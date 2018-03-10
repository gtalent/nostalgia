cmake_minimum_required(VERSION 2.8.8)

project(nostalgia)

set(NOSTALGIA_BUILD_TYPE "Native" CACHE STRING "The type of build to produce(Native/GBA)")
set(NOSTALGIA_IDE_BUILD "ON" CACHE STRING "Build for IDE's to run")
set(NOSTALGIA_QT_PATH "" CACHE STRING "Path to Qt Libraries")

set(CMAKE_PREFIX_PATH ${NOSTALGIA_QT_PATH})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(
		-DDEBUG
	)
else()
	add_definitions(
		-DNDEBUG
		-Werror
	)
endif()

if(NOT MSVC)
	add_definitions(
		-Wall
		-Wsign-compare
	)


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

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		add_definitions(
			-Werror
			-O3
		)
	endif()

	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_definitions(
			-fdiagnostics-color # forces colored output when using ninja
		)
	endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOSTALGIA_IDE_BUILD STREQUAL "ON")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if(APPLE)
	set(CMAKE_MACOSX_RPATH OFF)
	set(CMAKE_INSTALL_NAME_DIR "@executable_path/../Library/nostalgia")
	set(NOSTALGIA_DIST_BIN              nostalgia-studio.app/Contents/MacOS)
	set(NOSTALGIA_DIST_LIB              nostalgia-studio.app/Contents/Library)
	set(NOSTALGIA_DIST_PLUGIN           nostalgia-studio.app/Contents/Plugins)
	set(NOSTALGIA_DIST_RESOURCES        nostalgia-studio.app/Contents/Resources)
	set(NOSTALGIA_DIST_MAC_APP_CONTENTS nostalgia-studio.app/Contents)
else()
	set(CMAKE_INSTALL_RPATH "$ORIGIN" "$ORIGIN/../lib/nostalgia")
	if(NOT ${NOSTALGIA_QT_PATH} STREQUAL "")
		set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} "${NOSTALGIA_QT_PATH}/lib")
	endif()
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(NOSTALGIA_DIST_BIN       bin)
	set(NOSTALGIA_DIST_LIB       lib)
	set(NOSTALGIA_DIST_PLUGIN    lib/nostalgia/plugins)
	set(NOSTALGIA_DIST_RESOURCES share)
endif()

enable_testing()

add_subdirectory(deps/ox)
include_directories(deps/ox/src)

add_subdirectory(src)
