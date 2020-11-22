add_executable(
	ClawTest
		tests.cpp
)

target_link_libraries(
	ClawTest
		OxClaw
)

add_test("Test ClawTest ClawHeaderReader" ClawTest ClawHeaderReader)
add_test("Test ClawTest ClawHeaderReader2" ClawTest ClawHeaderReader2)
add_test("Test ClawTest ClawWriter" ClawTest ClawWriter)
add_test("Test ClawTest ClawReader" ClawTest ClawReader)
