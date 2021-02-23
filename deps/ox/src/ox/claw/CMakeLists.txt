
add_library(
	OxClaw
		read.cpp
		write.cpp
)

target_link_libraries(
	OxClaw PUBLIC
		OxMetalClaw
		OxOrganicClaw
)

add_subdirectory(test)
