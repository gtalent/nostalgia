enable_language(C ASM)

add_library(
	GbaStartup
		gba_crt0.s
		cstartup.cpp
)

target_link_libraries(
	GbaStartup
		OxStd
)
