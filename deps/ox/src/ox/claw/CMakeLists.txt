
add_library(
	OxClaw
		read.cpp
		write.cpp
)

target_link_libraries(
	OxClaw
		OxMetalClaw
		OxOrganicClaw
)

add_subdirectory(test)
