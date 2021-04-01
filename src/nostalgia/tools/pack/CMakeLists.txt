
add_library(
	NostalgiaPack
		pack.cpp
)

target_link_libraries(
	NostalgiaPack PUBLIC
		OxClArgs
		OxClaw
		OxFS
		OxStd
		NostalgiaCommon
		NostalgiaCore
)

install(
	TARGETS
		NostalgiaPack
	ARCHIVE DESTINATION
		lib/nostalgia
)
