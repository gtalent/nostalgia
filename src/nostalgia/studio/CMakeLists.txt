cmake_minimum_required(VERSION 2.8.11)

project(nostalgia-studio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
	nostalgia-studio MACOSX_BUNDLE
		main.cpp
		mainwindow.cpp
)

target_link_libraries(
	nostalgia-studio
		NostalgiaStudio
		NostalgiaPack
)

if(APPLE)
	set_target_properties(nostalgia-studio PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
endif()

install(
	FILES
		nostalgia-studio.json
	DESTINATION
		${NOSTALGIA_DIST_RESOURCES}
)

install(
	TARGETS
		nostalgia-studio
	RUNTIME DESTINATION
		${NOSTALGIA_DIST_BIN}
	BUNDLE DESTINATION .
)

add_subdirectory(lib)
