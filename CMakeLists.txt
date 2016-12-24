cmake_minimum_required(VERSION 2.8.8)

project(wombat)

set(WOMBAT_BUILD_TYPE "Native" CACHE STRING "The type of build to produce(Native/GBA)")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

if (WOMBAT_BUILD_TYPE STREQUAL "GBA")
	include(GBA)
endif()

add_definitions(
	-std=c++11
	-Wall
	-Wsign-compare
	-nostdlib
	-fno-exceptions
	-fno-rtti
	#-g
	#-fcolor-diagnostics
	#--analyze
	#-Os # GCC size optimization flag
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(
		-Werror
	)
endif()

enable_testing()

add_subdirectory(src)
