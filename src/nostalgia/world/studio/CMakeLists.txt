
add_library(
	NostalgiaWorld-Studio OBJECT
		consts.cpp
		newworldwizard.cpp
		worldstudioplugin.cpp
		worldeditor.cpp
)

target_link_libraries(
	NostalgiaWorld-Studio
		NostalgiaStudio
)

install(
	TARGETS
		NostalgiaWorld-Studio
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_MODULE}
)
