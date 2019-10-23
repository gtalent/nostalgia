
add_library(
	NostalgiaPack SHARED
		pack.cpp
)

target_link_libraries(
	NostalgiaPack PUBLIC
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
