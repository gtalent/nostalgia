add_executable(
	McTest
		tests.cpp
)

target_link_libraries(
	McTest
		OxMetalClaw
)

add_test("Test\\ McTest\\ Writer" McTest MetalClawWriter)
add_test("Test\\ McTest\\ Reader" McTest MetalClawReader)
add_test("Test\\ McTest\\ encodeInteger" McTest encodeInteger)
add_test("Test\\ McTest\\ decodeInteger" McTest decodeInteger)
