add_executable(
	McTest
		tests.cpp
)

target_link_libraries(
	McTest
		OxMetalClaw
)

add_test("[ox/mc] McTest Writer" McTest MetalClawWriter)
add_test("[ox/mc] McTest Reader" McTest MetalClawReader)
add_test("[ox/mc] McTest MetalClawDef" McTest MetalClawDef)
add_test("[ox/mc] McTest encodeInteger" McTest encodeInteger)
add_test("[ox/mc] McTest decodeInteger" McTest decodeInteger)
