cmake_minimum_required(VERSION 2.8.11)

#setup libraries

if(NOT WOMBAT_BUILD_TYPE STREQUAL "GBA")
	find_package(Qt5Widgets)
endif()

find_package(Ox REQUIRED)
include_directories(${Ox_INCLUDE_DIRS})


#project packages

include_directories(".")

add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(player)

if(NOT WOMBAT_BUILD_TYPE STREQUAL "GBA")
	add_subdirectory(tools)
	add_subdirectory(studio)
endif()
