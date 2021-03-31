
add_library(
	NostalgiaWorld-Studio OBJECT
		consts.cpp
		newworldwizard.cpp
		worldstudioplugin.cpp
		worldeditor.cpp
)

target_link_libraries(
	NostalgiaWorld-Studio
		Qt6::Core
		Qt6::Widgets
		NostalgiaStudio
)

install(
	TARGETS
		NostalgiaWorld-Studio
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_MODULE}
)
