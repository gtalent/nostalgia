
#setup libraries

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	find_package(SDL2 CONFIG REQUIRED)
	if(NOSTALGIA_BUILD_STUDIO)
		find_package(Qt5Widgets REQUIRED)
		find_package(Qt5QuickWidgets REQUIRED)
	endif()
endif()

#project packages

add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(world)

if(NOSTALGIA_BUILD_PLAYER)
	add_subdirectory(player)
endif()

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_subdirectory(tools)
	if(NOSTALGIA_BUILD_STUDIO)
		add_subdirectory(studio)
	endif()
endif()
