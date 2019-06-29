cmake_minimum_required(VERSION 3.13)

project(Ox)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
include(address_sanitizer)

if(NOT DEFINED OX_RUN_TESTS)
	set(OX_RUN_TESTS ON)
endif()
if(NOT DEFINED OX_BUILD_EXEC)
	set(OX_BUILD_EXEC ON)
endif()
if(NOT DEFINED OX_USE_STDLIB)
	set(OX_USE_STDLIB ON)
endif()
if(NOT DEFINED OX_BARE_METAL)
	set(OX_BARE_METAL OFF)
endif()

set(OX_RUN_TESTS ${OX_RUN_TESTS} CACHE BOOL "Run tests (ON/OFF)")
set(OX_BUILD_EXEC ${OX_BUILD_EXEC} CACHE BOOL "Build executables (ON/OFF)")
set(OX_USE_STDLIB ${OX_USE_STDLIB} CACHE BOOL "Build libraries that need the std lib (ON/OFF)")
set(OX_BARE_METAL ${OX_BARE_METAL} CACHE BOOL "Bare metal build (TRUE/FALSE)")

# can't run tests without building them
if(NOT OX_BUILD_EXEC OR NOT OX_USE_STDLIB)
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

if(NOT OX_USE_STDLIB)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib")
endif()

if(NOT MSVC)
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
