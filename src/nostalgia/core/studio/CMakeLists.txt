cmake_minimum_required(VERSION 2.8.11)

if(NOSTALGIA_BUILD_TYPE STREQUAL "Native")
	add_library(
		NostalgiaCore-Studio SHARED
			import_tilesheet_wizard.cpp
			plugin.cpp
	)
endif()

target_link_libraries(
	NostalgiaCore-Studio
		Qt5::Core
		Qt5::Widgets
		NostalgiaStudio
		OxFS
		OxStd
)

install(
	TARGETS
		NostalgiaCore-Studio
	LIBRARY DESTINATION
		lib/nostalgia
)

install(
	FILES
		charset.png
	DESTINATION
		share/core
)
