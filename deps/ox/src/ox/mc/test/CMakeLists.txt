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
