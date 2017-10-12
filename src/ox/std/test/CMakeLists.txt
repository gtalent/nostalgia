cmake_minimum_required(VERSION 2.8)

add_executable(
	StdTest
		tests.cpp
)

target_link_libraries(StdTest OxLog OxStd)

add_test("Test\\ ox_memcmp\\ ABCDEFG\\ !=\\ HIJKLMN" StdTest "ABCDEFG != HIJKLMN")
add_test("Test\\ ox_memcmp\\ HIJKLMN\\ !=\\ ABCDEFG" StdTest "HIJKLMN != ABCDEFG")
add_test("Test\\ ox_memcmp\\ ABCDEFG\\ ==\\ ABCDEFG" StdTest "ABCDEFG == ABCDEFG")
add_test("Test\\ ox_memcmp\\ ABCDEFGHI\\ ==\\ ABCDEFG" StdTest "ABCDEFGHI == ABCDEFG")


################################################################################
# StrOps Tests

add_executable(
	StrOpsTest
		strops_test.cpp
)

target_link_libraries(
	StrOpsTest
		OxStd
		OxLog
)

add_test("Test\\ ox_strcmp\\ asdf\\ !=\\ hijk" StrOpsTest "asdf < hijk")
add_test("Test\\ ox_strcmp\\ hijk\\ !=\\ asdf" StrOpsTest "hijk > asdf")
add_test("Test\\ ox_strcmp\\ read\\ !=\\ resize" StrOpsTest "read < resize")
add_test("Test\\ ox_strcmp\\ resize\\ !=\\ read" StrOpsTest "resize > read")
add_test("Test\\ ox_strcmp\\ resize\\ ==\\ resize" StrOpsTest "resize == resize")
add_test("Test\\ ox_strcmp\\ ''\\ ==\\ ''" StrOpsTest " == ")
add_test("Test\\ ox_strchr\\ 0" StrOpsTest "ox_strchr 0")
add_test("Test\\ ox_lastIndexOf\\ aaaa\\ a" StrOpsTest "ox_lastIndexOf aaaa a")


################################################################################
# Byte Swap Tests

add_executable(
	ByteSwapTest
		byteswap_test.cpp
)

target_link_libraries(
	ByteSwapTest
		OxStd
		OxLog
)

add_test("Test\\ bigEndianAdapt\\ 0x00ff" ByteSwapTest bigEndianAdapt<uint16_t> 0x00ff)
add_test("Test\\ bigEndianAdapt\\ 0xff00" ByteSwapTest bigEndianAdapt<uint16_t> 0xff00)

add_test("Test\\ bigEndianAdapt\\ 0x000000ff" ByteSwapTest bigEndianAdapt<uint32_t> 0x000000ff)
add_test("Test\\ bigEndianAdapt\\ 0x0000ff00" ByteSwapTest bigEndianAdapt<uint32_t> 0x0000ff00)
add_test("Test\\ bigEndianAdapt\\ 0x00ff0000" ByteSwapTest bigEndianAdapt<uint32_t> 0x00ff0000)
add_test("Test\\ bigEndianAdapt\\ 0xff000000" ByteSwapTest bigEndianAdapt<uint32_t> 0xff000000)

add_test("Test\\ bigEndianAdapt\\ 0x00000000000000ff" ByteSwapTest bigEndianAdapt<uint64_t> 0x00000000000000ff)
add_test("Test\\ bigEndianAdapt\\ 0x000000000000ff00" ByteSwapTest bigEndianAdapt<uint64_t> 0x000000000000ff00)
add_test("Test\\ bigEndianAdapt\\ 0x0000000000ff0000" ByteSwapTest bigEndianAdapt<uint64_t> 0x0000000000ff0000)
add_test("Test\\ bigEndianAdapt\\ 0x00000000ff000000" ByteSwapTest bigEndianAdapt<uint64_t> 0x00000000ff000000)
add_test("Test\\ bigEndianAdapt\\ 0x000000ff00000000" ByteSwapTest bigEndianAdapt<uint64_t> 0x000000ff00000000)
add_test("Test\\ bigEndianAdapt\\ 0x0000ff0000000000" ByteSwapTest bigEndianAdapt<uint64_t> 0x0000ff0000000000)
add_test("Test\\ bigEndianAdapt\\ 0x00ff000000000000" ByteSwapTest bigEndianAdapt<uint64_t> 0x00ff000000000000)
add_test("Test\\ bigEndianAdapt\\ 0xff00000000000000" ByteSwapTest bigEndianAdapt<uint64_t> 0xff00000000000000)
