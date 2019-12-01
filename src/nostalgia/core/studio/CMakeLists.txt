add_library(
	NostalgiaCore-Studio SHARED
		imgconv.cpp
		import_tilesheet_wizard.cpp
		new_tilesheet_wizard.cpp
		plugin.cpp
		tilesheeteditor.cpp
		rsrc.qrc
)

target_link_libraries(
	NostalgiaCore-Studio
		Qt5::Core
		Qt5::Widgets
		Qt5::QuickWidgets
		NostalgiaStudio
		NostalgiaCore-Qt
		NostalgiaCore
		OxFS
		OxStd
)

target_compile_definitions(NostalgiaCore-Studio PRIVATE QT_QML_DEBUG)

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
