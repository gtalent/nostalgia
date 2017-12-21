cmake_minimum_required(VERSION 2.8.11)

project(nostalgia-studio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(
	nostalgia-studio
		main.cpp
		mainwindow.cpp
)

target_link_libraries(
	nostalgia-studio
		Qt5::Core
		Qt5::Widgets
		OxClArgs
		OxFS
		OxStd
		NostalgiaCommon
		NostalgiaCore
		NostalgiaStudio
		NostalgiaTool
)

if(APPLE)
	install(
		FILES
			Info.plist
		DESTINATION
			${NOSTALGIA_DIST_MAC_APP_CONTENTS}
	)
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
)

add_subdirectory(lib)
