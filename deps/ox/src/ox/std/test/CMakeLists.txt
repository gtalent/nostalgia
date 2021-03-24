cmake_minimum_required(VERSION 3.10)

add_executable(
	StdTest
		tests.cpp
)

target_link_libraries(StdTest OxStd)

add_test("[ox/std] ox_memcmp ABCDEFG != HIJKLMN" StdTest "ABCDEFG != HIJKLMN")
add_test("[ox/std] ox_memcmp HIJKLMN != ABCDEFG" StdTest "HIJKLMN != ABCDEFG")
add_test("[ox/std] ox_memcmp ABCDEFG == ABCDEFG" StdTest "ABCDEFG == ABCDEFG")
add_test("[ox/std] ox_memcmp ABCDEFGHI == ABCDEFG" StdTest "ABCDEFGHI == ABCDEFG")
add_test("[ox/std] BString" StdTest "BString")
add_test("[ox/std] String" StdTest "String")
add_test("[ox/std] Vector" StdTest "Vector")
add_test("[ox/std] HashMap" StdTest "HashMap")
add_test("[ox/std] HeapMgr" StdTest malloc)
