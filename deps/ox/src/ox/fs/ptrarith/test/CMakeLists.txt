add_executable(
	PtrArithTests
		tests.cpp
)

target_link_libraries(
	PtrArithTests
		OxStd
)

add_test("[ox/fs] PtrArith::setSize" PtrArithTests PtrArith::setSize)
