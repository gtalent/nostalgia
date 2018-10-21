cmake_minimum_required(VERSION 2.8)

project(Ox)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
include(address_sanitizer)

set(OX_BUILD_EXEC "ON" CACHE STRING "Build executables (ON/OFF)")
set(OX_RUN_TESTS  "ON" CACHE STRING "Run tests (ON/OFF)")
set(OX_USE_STDLIB "ON" CACHE STRING "Build libraries that need the std lib (ON/OFF)")

# can't run tests without building them
if(OX_BUILD_EXEC STREQUAL "OFF" OR OX_USE_STDLIB STREQUAL "OFF")
	set(OX_BUILD_EXEC OFF)
	set(OX_RUN_TESTS OFF)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-DNDEBUG)
else()
	add_definitions(-DDEBUG)
endif()

if(OX_USE_STDLIB STREQUAL "ON")
	add_definitions(-DOX_USE_STDLIB)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib")
endif()

if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-variable")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-field-initializers")
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	endif()
	# forces colored output when using ninja
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
endif()

enable_testing()

include_directories("src")

install(FILES OxConfig.cmake DESTINATION lib/ox)

add_subdirectory(src)
