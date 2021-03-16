cmake_minimum_required(VERSION 3.19)
set(CMAKE_POLICY_DEFAULT_CMP0110 NEW) # requires CMake 3.19

project(nostalgia)

set(QTDIR "" CACHE PATH "Path to Qt Libraries")

set(BUILDCORE_TARGET "Native" CACHE STRING "The type of build to produce(Native/GBA)")
set(NOSTALGIA_BUILD_PLAYER ON CACHE BOOL "Build Player")
set(NOSTALGIA_BUILD_STUDIO ON CACHE BOOL "Build Studio")
set(OX_ENABLE_TRACEHOOK OFF CACHE BOOL "Generate OxTraceHook shared library for uprobes")

if(BUILDCORE_TARGET STREQUAL "gba")
	set(NOSTALGIA_BUILD_TYPE "GBA")
else()
	set(NOSTALGIA_BUILD_TYPE "Native")
endif()

if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	set(NOSTALGIA_BUILD_STUDIO OFF)
	set(OX_BARE_METAL ON)
	set(OX_USE_STDLIB OFF)
else()
	set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QTDIR})
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()


list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
include(GenerateExportHeader)
include(address_sanitizer)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist/${BUILDCORE_BUILD_CONFIG}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT OX_BARE_METAL)
	add_definitions(-DOX_USE_STDLIB)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# enable ccache
if(NOT DEFINED ENV{NOSTALGIA_SUPPRESS_CCACHE})
	find_program(CCACHE_PROGRAM ccache)
	if(CCACHE_PROGRAM)
		 set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
	endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
else()
	add_definitions(-DNDEBUG)
endif()

if(NOT MSVC)
	# forces colored output when using ninja
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")

	# enable warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat=2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-field-initializers")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnull-dereference")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-conversion")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable")

	# disable warnings
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")

	if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
		include(GBA)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-unwind-tables")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb-interwork")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=arm7tdmi")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=arm7tdmi")
	endif()

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	endif()
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

enable_testing()

add_subdirectory(deps/ox)
include_directories(SYSTEM deps/ox/src)

if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	add_subdirectory(deps/gbastartup)
elseif(NOSTALGIA_BUILD_STUDIO)
	add_subdirectory(deps/QDark)
endif()

add_subdirectory(src)
