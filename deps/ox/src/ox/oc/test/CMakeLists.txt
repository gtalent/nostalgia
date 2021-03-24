add_executable(
	OcTest
		tests.cpp
)

target_link_libraries(
	OcTest
		OxOrganicClaw
)

add_test("[ox/oc] OcTest Writer" OcTest OrganicClawWriter)
add_test("[ox/oc] OcTest Reader" OcTest OrganicClawReader)
add_test("[ox/oc] OcTest OrganicClawDef" OcTest OrganicClawDef)
