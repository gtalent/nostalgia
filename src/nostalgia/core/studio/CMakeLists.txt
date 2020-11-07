add_library(
	NostalgiaCore-Studio SHARED
		imgconv.cpp
		import_tilesheet_wizard.cpp
		new_tilesheet_wizard.cpp
		newpalettewizard.cpp
		paletteeditor.cpp
		plugin.cpp
		tilesheeteditor.cpp
		util.cpp
		rsrc.qrc
)

if(NOT MSVC)
	target_compile_options(NostalgiaCore-Studio PRIVATE -Wsign-conversion)
endif()

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

#target_compile_definitions(NostalgiaCore-Studio PRIVATE QT_QML_DEBUG)

install(
	TARGETS
		NostalgiaCore-Studio
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_PLUGIN}
)
