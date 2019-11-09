add_library(
	NostalgiaCore-GBA
		core.cpp
		gfx.cpp
		media.cpp
		mem.cpp
		panic.cpp
)

target_link_libraries(
	NostalgiaCore-GBA PUBLIC
		NostalgiaCore
		GbaStartup
		OxFS
		OxStd
)

install(
	TARGETS
		NostalgiaCore-GBA
	DESTINATION
		include/nostalgia/core
)
