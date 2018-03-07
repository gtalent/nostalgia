cmake_minimum_required(VERSION 2.8)

project(Ox)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
include(address_sanitizer)

set(OX_BUILD_EXEC "ON" CACHE STRING "Build executables (ON/OFF)")
set(OX_RUN_TESTS  "ON" CACHE STRING "Run tests (ON/OFF)")
set(OX_USE_STDLIB "ON" CACHE STRING "Build libraries that need the std lib (ON/OFF)")

# can't run tests without building them
if(OX_BUILD_EXEC STREQUAL "OFF" OR OX_USE_STDLIB STREQUAL "OFF")
	set(OX_BUILD_EXEC "OFF")
	set(OX_RUN_TESTS "OFF")
endif()

if(OX_USE_STDLIB STREQUAL "ON")
	add_definitions(-DOX_USE_STDLIB)
endif()

if(NOT MSVC)
	add_definitions(
		-std=c++11
		-Wall
		-nostdlib
		-fno-exceptions
		-fno-rtti
		-Wsign-compare
		-Wunused-variable
		#--analyze
		#-Os # GCC size optimization flag
	)

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		add_definitions(
			-Werror
		)
	endif()
endif(NOT MSVC)

enable_testing()

include_directories("src")

install(FILES OxConfig.cmake DESTINATION lib/ox)

add_subdirectory(src)
