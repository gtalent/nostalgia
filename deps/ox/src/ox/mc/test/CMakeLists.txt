cmake_minimum_required(VERSION 2.8)

add_executable(
	McTest
		tests.cpp
)

target_link_libraries(
	McTest
		OxMetalClaw
		OxStd
		OxTrace
)

add_test("Test\\ McTest\\ Writer" McTest MetalClawWriter)
add_test("Test\\ McTest\\ Reader" McTest MetalClawReader)
