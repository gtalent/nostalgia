
#setup libraries

if(NOSTALGIA_BUILD_STUDIO)
	find_package(Qt5Widgets)
	set(CMAKE_AUTOMOC ON)
endif()

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	find_package(SDL2 REQUIRED)
endif()

#project packages

add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(player)
add_subdirectory(world)

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_subdirectory(tools)
endif()

if(NOSTALGIA_BUILD_STUDIO)
	add_subdirectory(studio)
endif()
