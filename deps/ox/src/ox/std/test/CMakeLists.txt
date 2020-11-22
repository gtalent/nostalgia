cmake_minimum_required(VERSION 3.10)

add_executable(
	StdTest
		tests.cpp
)

target_link_libraries(StdTest OxStd)

add_test("Test ox_memcmp ABCDEFG != HIJKLMN" StdTest "ABCDEFG != HIJKLMN")
add_test("Test ox_memcmp HIJKLMN != ABCDEFG" StdTest "HIJKLMN != ABCDEFG")
add_test("Test ox_memcmp ABCDEFG == ABCDEFG" StdTest "ABCDEFG == ABCDEFG")
add_test("Test ox_memcmp ABCDEFGHI == ABCDEFG" StdTest "ABCDEFGHI == ABCDEFG")
add_test("Test BString" StdTest "BString")
add_test("Test String" StdTest "String")
add_test("Test Vector" StdTest "Vector")
add_test("Test HashMap" StdTest "HashMap")
add_test("Test HeapMgr" StdTest malloc)
