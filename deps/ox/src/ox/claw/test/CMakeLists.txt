add_executable(
	ClawTest
		tests.cpp
)

target_link_libraries(
	ClawTest
		OxClaw
)

add_test("[ox/claw] ClawTest ClawHeaderReader" ClawTest ClawHeaderReader)
add_test("[ox/claw] ClawTest ClawHeaderReader2" ClawTest ClawHeaderReader2)
add_test("[ox/claw] ClawTest ClawWriter" ClawTest ClawWriter)
add_test("[ox/claw] ClawTest ClawReader" ClawTest ClawReader)
