
add_library(
	NostalgiaPack
		pack.cpp
)

target_link_libraries(
	NostalgiaPack PUBLIC
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
	ARCHIVE DESTINATION
		${NOSTALGIA_DIST_LIB}/nostalgia
)
