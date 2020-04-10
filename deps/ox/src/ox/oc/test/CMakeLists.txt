add_executable(
	OcTest
		tests.cpp
)

target_link_libraries(
	OcTest
		OxOrganicClaw
)

add_test("Test\\ OcTest\\ Writer" OcTest OrganicClawWriter)
add_test("Test\\ OcTest\\ Reader" OcTest OrganicClawReader)
add_test("Test\\ OcTest\\ OrganicClawDef" OcTest OrganicClawDef)
