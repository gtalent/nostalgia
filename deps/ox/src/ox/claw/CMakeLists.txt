
add_library(
	OxClaw
		read.cpp
		write.cpp
)

target_link_libraries(
	OxClaw PUBLIC
		OxMetalClaw
		$<$<BOOL:OX_USE_STDLIB>:OxOrganicClaw>
)

if(OX_RUN_TESTS)
	add_subdirectory(test)
endif()