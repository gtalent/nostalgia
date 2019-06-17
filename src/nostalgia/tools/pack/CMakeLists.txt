
add_library(
	NostalgiaPack SHARED
		imgconv.cpp
		pack.cpp
)

target_link_libraries(
	NostalgiaPack
		Qt5::Widgets
		OxClArgs
		OxFS
		OxStd
		OxMetalClaw
		NostalgiaCommon
		NostalgiaCore
)

install(
	TARGETS
		NostalgiaPack
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_LIB}/nostalgia
)
