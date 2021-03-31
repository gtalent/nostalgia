
#setup libraries

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	if(NOSTALGIA_BUILD_STUDIO)
		find_package(Qt6Widgets REQUIRED)
		find_package(Qt6QuickWidgets REQUIRED)
	endif()
endif()

#project packages

add_subdirectory(core)
add_subdirectory(common)
add_subdirectory(scene)
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
