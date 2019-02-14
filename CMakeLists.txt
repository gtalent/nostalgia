cmake_minimum_required(VERSION 2.8.8)

project(nostalgia)

set(NOSTALGIA_BUILD_TYPE "Native" CACHE STRING "The type of build to produce(Native/GBA)")
set(NOSTALGIA_IDE_BUILD "ON" CACHE STRING "Build for IDE's to run")
set(NOSTALGIA_QT_PATH "" CACHE STRING "Path to Qt Libraries")

if(NOSTALGIA_QT_PATH STREQUAL "" OR NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	SET(NOSTALGIA_BUILD_STUDIO FALSE)
else()
	SET(NOSTALGIA_BUILD_STUDIO TRUE)
	set(CMAKE_PREFIX_PATH ${NOSTALGIA_QT_PATH})
endif()


list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
else()
	add_definitions(-DNDEBUG)
endif()

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")

	if(NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
		include(GBA)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb-interwork")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb")

		set(OX_USE_STDLIB OFF)
	endif()

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	endif()

	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		# forces colored output when using ninja
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	endif()
endif()

if(NOSTALGIA_IDE_BUILD STREQUAL "ON")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if(NOT NOSTALGIA_BUILD_TYPE STREQUAL "GBA")
	add_definitions(-DOX_USE_STDLIB)
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
