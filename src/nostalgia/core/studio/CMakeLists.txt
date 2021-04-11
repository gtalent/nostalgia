
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(
	NostalgiaCore-Studio OBJECT
		imgconv.cpp
		import_tilesheet_wizard.cpp
		module.cpp
		new_tilesheet_wizard.cpp
		newpalettewizard.cpp
		paletteeditor.cpp
		tilesheeteditor.cpp
		util.cpp
		rsrc.qrc
)

if(NOT MSVC)
	target_compile_options(NostalgiaCore-Studio PRIVATE -Wsign-conversion)
endif()

target_precompile_headers(
	NostalgiaCore-Studio PRIVATE
		<QBuffer>
		<QButtonGroup>
		<QColor>
		<QDebug>
		<QDialog>
		<QFile>
		<QFormLayout>
		<QHeaderView>
		<QImage>
		<QItemDelegate>
		<QMap>
		<QPainter>
		<QPluginLoader>
		<QPointer>
		<QPushButton>
		<QQmlContext>
		<QQuickItem>
		<QQuickWidget>
		<QSettings>
		<QSpinBox>
		<QSplitter>
		<QString>
		<QStringList>
		<QStyledItemDelegate>
		<QTableWidget>
		<QToolBar>
		<QUndoCommand>
		<QUndoStack>
		<QVariant>
)

target_link_libraries(
	NostalgiaCore-Studio
		Qt${QT_VERSION_MAJOR}::QuickWidgets
		NostalgiaStudio
		NostalgiaCore-Qt
		NostalgiaCore
)

#target_compile_definitions(NostalgiaCore-Studio PRIVATE QT_QML_DEBUG)

install(
	TARGETS
		NostalgiaCore-Studio
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_MODULE}
)
