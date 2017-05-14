cmake_minimum_required(VERSION 2.8.11)

project(nostalgia-studio)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

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

install(
	TARGETS
		nostalgia-studio
	RUNTIME DESTINATION
		bin
)

add_subdirectory(lib)
