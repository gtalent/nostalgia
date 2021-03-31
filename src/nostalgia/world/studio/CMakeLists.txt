
add_library(
	NostalgiaWorld-Studio OBJECT
		consts.cpp
		newworldwizard.cpp
		worldstudioplugin.cpp
		worldeditor.cpp
)

target_link_libraries(
	NostalgiaWorld-Studio
		Qt${QT_VERSION_MAJOR}::Core
		Qt${QT_VERSION_MAJOR}::Widgets
		NostalgiaStudio
)

install(
	TARGETS
		NostalgiaWorld-Studio
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_MODULE}
)
