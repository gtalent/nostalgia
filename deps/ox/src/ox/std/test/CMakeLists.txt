cmake_minimum_required(VERSION 2.8)

add_executable(
	StdTest
		tests.cpp
)

target_link_libraries(StdTest OxTrace OxStd)

add_test("Test\\ ox_memcmp\\ ABCDEFG\\ !=\\ HIJKLMN" StdTest "ABCDEFG != HIJKLMN")
add_test("Test\\ ox_memcmp\\ HIJKLMN\\ !=\\ ABCDEFG" StdTest "HIJKLMN != ABCDEFG")
add_test("Test\\ ox_memcmp\\ ABCDEFG\\ ==\\ ABCDEFG" StdTest "ABCDEFG == ABCDEFG")
add_test("Test\\ ox_memcmp\\ ABCDEFGHI\\ ==\\ ABCDEFG" StdTest "ABCDEFGHI == ABCDEFG")
add_test("Test\\ Vector" StdTest "Vector")
add_test("Test\\ HashMap" StdTest "HashMap")
