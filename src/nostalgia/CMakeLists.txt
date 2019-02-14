
#setup libraries

if(NOSTALGIA_BUILD_STUDIO)
	find_package(Qt5Widgets)
	set(CMAKE_AUTOMOC ON)
endif()

#project packages

add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(player)
add_subdirectory(world)

if(NOSTALGIA_BUILD_STUDIO)
	add_subdirectory(tools)
	add_subdirectory(studio)
endif()
