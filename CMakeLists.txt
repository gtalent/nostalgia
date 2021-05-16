cmake_minimum_required(VERSION 3.19)
set(CMAKE_POLICY_DEFAULT_CMP0110 NEW) # requires CMake 3.19

project(nostalgia)

include(deps/buildcore/base.cmake)

set(NOSTALGIA_BUILD_PLAYER ON CACHE BOOL "Build Player")
set(NOSTALGIA_BUILD_STUDIO ON CACHE BOOL "Build Studio")
set(OX_ENABLE_TRACEHOOK OFF CACHE BOOL "Generate OxTraceHook shared library for uprobes")

if(BUILDCORE_TARGET STREQUAL "gba")
	set(NOSTALGIA_BUILD_STUDIO OFF)
	set(NOSTALGIA_BUILD_TYPE "GBA")
	include(deps/gbabuildcore/base.cmake)
else()
	set(NOSTALGIA_BUILD_TYPE "Native")
	set(NOSTALGIA_CONAN_PATHS ${CMAKE_SOURCE_DIR}/.conanbuild/conan_paths.cmake)
	if(EXISTS ${NOSTALGIA_CONAN_PATHS})
		include(${NOSTALGIA_CONAN_PATHS})
	endif()
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QTDIR})
endif()

if(APPLE)
	set(CMAKE_MACOSX_RPATH OFF)
	set(CMAKE_INSTALL_NAME_DIR "@executable_path/../Library/nostalgia")
	set(NOSTALGIA_DIST_BIN              nostalgia-studio.app/Contents/MacOS)
	set(NOSTALGIA_DIST_LIB              nostalgia-studio.app/Contents/Library)
	set(NOSTALGIA_DIST_MODULE           nostalgia-studio.app/Contents/Plugins)
	set(NOSTALGIA_DIST_RESOURCES        nostalgia-studio.app/Contents/Resources)
	set(NOSTALGIA_DIST_MAC_APP_CONTENTS nostalgia-studio.app/Contents)
else()
	set(CMAKE_INSTALL_RPATH "$ORIGIN" "$ORIGIN/../lib/ox" "$ORIGIN/../lib/nostalgia" "$ORIGIN/../")
	if(QTDIR)
		set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} "${QTDIR}/lib")
	endif()
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(NOSTALGIA_DIST_BIN       bin)
	set(NOSTALGIA_DIST_LIB       lib)
	set(NOSTALGIA_DIST_MODULE    lib/nostalgia/modules)
	set(NOSTALGIA_DIST_RESOURCES share)
endif()

add_subdirectory(deps/ox)
include_directories(
	SYSTEM
		deps/imgui/src
		deps/ox/src
)

if(BUILDCORE_TARGET STREQUAL "gba")
	add_subdirectory(deps/gbastartup)
else()
	if(NOSTALGIA_BUILD_STUDIO)
		add_subdirectory(deps/QDark)
	endif()
	add_subdirectory(deps/imgui)
endif()

add_subdirectory(src)
