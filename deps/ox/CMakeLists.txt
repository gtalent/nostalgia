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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(OX_USE_STDLIB STREQUAL "ON")
	add_definitions(-DOX_USE_STDLIB)
endif()

if(NOT MSVC)
	add_definitions(
		-std=c++17
		-nostdlib
		-fno-exceptions
		-fno-rtti
		-Wall
		-Wextra
		-Wsign-compare
		-Wunused-variable
	)
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

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(
		-DNDEBUG
	)
else()
	add_definitions(
		-DDEBUG
	)
endif()

enable_testing()

include_directories("src")

install(FILES OxConfig.cmake DESTINATION lib/ox)

add_subdirectory(src)
