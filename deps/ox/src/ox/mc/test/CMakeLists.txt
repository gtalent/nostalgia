cmake_minimum_required(VERSION 2.8)

add_executable(
	McTest
		tests.cpp
)

target_link_libraries(McTest OxStd OxMetalClaw)

add_test("Test\\ McTest\\ Writer" McTest MetalClawWriter)
add_test("Test\\ McTest\\ Reader" McTest MetalClawReader)
