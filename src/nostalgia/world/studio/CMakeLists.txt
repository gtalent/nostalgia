
add_library(
	NostalgiaWorld-Studio SHARED
		consts.cpp
		newworldwizard.cpp
		worldstudioplugin.cpp
		worldeditor.cpp
)

target_link_libraries(
	NostalgiaWorld-Studio
		Qt5::Core
		Qt5::Widgets
		NostalgiaStudio
)

install(
	TARGETS
		NostalgiaWorld-Studio
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_PLUGIN}/nostalgia
)
