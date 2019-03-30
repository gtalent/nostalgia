
add_library(
	Pack SHARED
		imgconv.cpp
		pack.cpp
)

target_link_libraries(
	Pack
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
		Pack
	LIBRARY DESTINATION
		${NOSTALGIA_DIST_LIB}/nostalgia
)
