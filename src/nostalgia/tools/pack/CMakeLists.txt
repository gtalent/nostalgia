
add_library(
	NostalgiaPack
		pack.cpp
)

target_link_libraries(
	NostalgiaPack PUBLIC
		OxClaw
		OxFS
		NostalgiaCommon
		NostalgiaCore
)

install(
	TARGETS
		NostalgiaPack
	ARCHIVE DESTINATION
		lib/nostalgia
)
