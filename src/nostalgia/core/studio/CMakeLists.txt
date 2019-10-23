
add_library(
	NostalgiaCore-Studio SHARED
		imgconv.cpp
		import_tilesheet_wizard.cpp
		new_tilesheet_wizard.cpp
		plugin.cpp
)

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
		${NOSTALGIA_DIST_PLUGIN}
)

install(
	FILES
		charset.png
	DESTINATION
		share/core
)
