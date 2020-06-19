/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "ox/std/hashmap.hpp"
#undef NDEBUG

#include <assert.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ox/mc/mc.hpp>
#include <ox/model/model.hpp>
#include <ox/std/std.hpp>

union TestUnion {
	static constexpr auto TypeName = "TestUnion";
	static constexpr auto Fields = 3;
	bool Bool;
	uint32_t Int = 5;
	char String[32];
};

struct TestStructNest {
	static constexpr auto TypeName = "TestStructNest";
	static constexpr auto Fields = 3;
	bool Bool = false;
	uint32_t Int = 0;
	ox::BString<32> String = "";
};

struct TestStruct {
	static constexpr auto TypeName = "TestStruct";
	static constexpr auto Fields = 17;
	bool Bool = false;
	int32_t Int = 0;
	int32_t Int1 = 0;
	int32_t Int2 = 0;
	int32_t Int3 = 0;
	int32_t Int4 = 0;
	int32_t Int5 = 0;
	int32_t Int6 = 0;
	int32_t Int7 = 0;
	int32_t Int8 = 0;
	TestUnion Union;
	char *CString = nullptr;
	ox::BString<32> String = "";
	uint32_t List[4] = {0, 0, 0, 0};
	ox::HashMap<ox::String, int> Map;
	TestStructNest EmptyStruct;
	TestStructNest Struct;

	~TestStruct() {
		delete[] CString;
	}

};

template<typename T>
ox::Error model(T *io, TestUnion *obj) {
	io->template setTypeInfo<TestUnion>();
	oxReturnError(io->field("Bool", &obj->Bool));
	oxReturnError(io->field("Int", &obj->Int));
	oxReturnError(io->field("String", ox::SerStr(obj->String)));
	return OxError(0);
}

template<typename T>
ox::Error model(T *io, TestStructNest *obj) {
	io->template setTypeInfo<TestStructNest>();
	oxReturnError(io->field("Bool", &obj->Bool));
	oxReturnError(io->field("Int", &obj->Int));
	oxReturnError(io->field("String", &obj->String));
	return OxError(0);
}

template<typename T>
ox::Error model(T *io, TestStruct *obj) {
	io->template setTypeInfo<TestStruct>();
	oxReturnError(io->field("Bool", &obj->Bool));
	oxReturnError(io->field("Int", &obj->Int));
	oxReturnError(io->field("Int1", &obj->Int1));
	oxReturnError(io->field("Int2", &obj->Int2));
	oxReturnError(io->field("Int3", &obj->Int3));
	oxReturnError(io->field("Int4", &obj->Int4));
	oxReturnError(io->field("Int5", &obj->Int5));
	oxReturnError(io->field("Int6", &obj->Int6));
	oxReturnError(io->field("Int7", &obj->Int7));
	oxReturnError(io->field("Int8", &obj->Int8));
	oxReturnError(io->field("Union", ox::UnionView{&obj->Union, 1}));
	oxReturnError(io->field("CString", ox::SerStr(&obj->CString)));
	oxReturnError(io->field("String", &obj->String));
	oxReturnError(io->field("List", obj->List, 4));
	oxReturnError(io->field("Map", &obj->Map));
	oxReturnError(io->field("EmptyStruct", &obj->EmptyStruct));
	oxReturnError(io->field("Struct", &obj->Struct));
	return OxError(0);
}

std::map<std::string, ox::Error(*)()> tests = {
	{
		{
			"MetalClawWriter",
			[] {
				// This test doesn't confirm much, but it does show that the writer
				// doesn't segfault
				constexpr size_t buffLen = 1024;
				uint8_t buff[buffLen];
				TestStruct ts;

				oxReturnError(ox::writeMC(buff, buffLen, &ts));

				return OxError(0);
			}
		},
		{
			"MetalClawReader",
			[] {
				constexpr size_t buffLen = 1024;
				uint8_t buff[buffLen];
				TestStruct testIn, testOut;

				testIn.Bool = true;
				testIn.Int = 42;
				testIn.Union.Int = 42;
				testIn.String = "Test String 1";
				testIn.CString = new char[ox_strlen("c-string") + 1];
				ox_strcpy(testIn.CString, "c-string");
				testIn.List[0] = 1;
				testIn.List[1] = 2;
				testIn.List[2] = 3;
				testIn.List[3] = 4;
				testIn.Map["asdf"] = 93;
				testIn.Map["aoeu"] = 94;
				testIn.Struct.Bool = false;
				testIn.Struct.Int = 300;
				testIn.Struct.String = "Test String 2";

				oxAssert(ox::writeMC(buff, buffLen, &testIn), "writeMC failed");
				oxAssert(ox::readMC(buff, buffLen, &testOut), "writeMC failed");
				//std::cout << testIn.Union.Int << "|" << testOut.Union.Int << "|\n";

				oxAssert(testIn.Bool               == testOut.Bool, "Bool value mismatch");
				oxAssert(testIn.Int                == testOut.Int, "Int value mismatch");
				oxAssert(testIn.Int1               == testOut.Int1, "Int1 value mismatch");
				oxAssert(testIn.Int2               == testOut.Int2, "Int2 value mismatch");
				oxAssert(testIn.Int3               == testOut.Int3, "Int3 value mismatch");
				oxAssert(testIn.Int4               == testOut.Int4, "Int4 value mismatch");
				oxAssert(testIn.Int5               == testOut.Int5, "Int5 value mismatch");
				oxAssert(testIn.Int6               == testOut.Int6, "Int6 value mismatch");
				oxAssert(testIn.Int7               == testOut.Int7, "Int7 value mismatch");
				oxAssert(testIn.Int8               == testOut.Int8, "Int8 value mismatch");
				oxAssert(ox_strcmp(testIn.CString, testOut.CString) == 0, "CString value mismatch");
				oxAssert(testIn.Union.Int          == testOut.Union.Int, "Union.Int value mismatch");
				oxAssert(testIn.String             == testOut.String, "String value mismatch");
				oxAssert(testIn.List[0]            == testOut.List[0], "List[0] value mismatch");
				oxAssert(testIn.List[1]            == testOut.List[1], "List[1] value mismatch");
				oxAssert(testIn.List[2]            == testOut.List[2], "List[2] value mismatch");
				oxAssert(testIn.List[3]            == testOut.List[3], "List[3] value mismatch");
				oxAssert(testIn.Map["asdf"]        == testOut.Map["asdf"], "Map[\"asdf\"] value mismatch");
				oxAssert(testIn.Map["aoeu"]        == testOut.Map["aoeu"], "Map[\"aoeu\"] value mismatch");
				oxAssert(testIn.EmptyStruct.Bool   == testOut.EmptyStruct.Bool, "EmptyStruct.Bool value mismatch");
				oxAssert(testIn.EmptyStruct.Int    == testOut.EmptyStruct.Int, "EmptyStruct.Int value mismatch");
				oxAssert(testIn.EmptyStruct.String == testOut.EmptyStruct.String, "EmptyStruct.String value mismatch");
				oxAssert(testIn.Struct.Int         == testOut.Struct.Int, "Struct.Int value mismatch");
				oxAssert(testIn.Struct.String      == testOut.Struct.String, "Struct.String value mismatch");
				oxAssert(testIn.Struct.Bool        == testOut.Struct.Bool, "Struct.Bool value mismatch");

				return OxError(0);
			}
		},
		{
			"encodeInteger",
			[] {
				using ox::MaxValue;
				using ox::mc::McInt;
				using ox::mc::encodeInteger;

				constexpr auto check = [](McInt val, std::vector<uint8_t> &&expected) {
					if (val.length != expected.size()) {
						std::cout << "val.length: " << val.length << ", expected: " << expected.size() << '\n';
						return OxError(1);
					}
					for (std::size_t i = 0; i < expected.size(); i++) {
						if (expected[i] != val.data[i]) {
							std::cout << i << ": " << static_cast<uint32_t>(val.data[i]) << '\n';
							return OxError(1);
						}
					}
					return OxError(0);
				};
				constexpr auto check64 = [](McInt val, auto expected) {
					if (val.length != 9) {
						std::cout << "val.length: " << val.length << '\n';
						return OxError(1);
					}
					ox::LittleEndian<decltype(expected)> decoded = *reinterpret_cast<decltype(expected)*>(&val.data[1]);
					if (expected != decoded) {
						std::cout << "decoded: " << decoded << ", expected: " << expected << '\n';
						return OxError(1);
					}
					return OxError(0);
				};

				oxAssert(check(encodeInteger(int64_t(1)), {0b00000010}), "Encode 1 fail");
				oxAssert(check(encodeInteger(int64_t(2)), {0b00000100}), "Encode 2 fail");
				oxAssert(check(encodeInteger(int64_t(3)), {0b00000110}), "Encode 3 fail");
				oxAssert(check(encodeInteger(int64_t(4)), {0b00001000}), "Encode 4 fail");
				oxAssert(check(encodeInteger(int64_t(128)), {0b00000001, 0b10}), "Encode 128 fail");
				oxAssert(check(encodeInteger(int64_t(129)), {0b00000101, 0b10}), "Encode 129 fail");
				oxAssert(check(encodeInteger(int64_t(130)), {0b00001001, 0b10}), "Encode 130 fail");
				oxAssert(check(encodeInteger(int64_t(131)), {0b00001101, 0b10}), "Encode 131 fail");

				oxAssert(check(encodeInteger(int64_t(-1)), {255, 255, 255, 255, 255, 255, 255, 255, 255}), "Encode -1 fail");
				oxAssert(check(encodeInteger(int64_t(-2)), {255, 254, 255, 255, 255, 255, 255, 255, 255}), "Encode -2 fail");
				oxAssert(check(encodeInteger(int64_t(-3)), {255, 253, 255, 255, 255, 255, 255, 255, 255}), "Encode -3 fail");
				oxAssert(check(encodeInteger(int64_t(-4)), {255, 252, 255, 255, 255, 255, 255, 255, 255}), "Encode -4 fail");
				oxAssert(check(encodeInteger(int64_t(-128)), {255, 128, 255, 255, 255, 255, 255, 255, 255}), "Encode -128 fail");
				oxAssert(check(encodeInteger(int64_t(-129)), {255, 127, 255, 255, 255, 255, 255, 255, 255}), "Encode -129 fail");
				oxAssert(check(encodeInteger(int64_t(-130)), {255, 126, 255, 255, 255, 255, 255, 255, 255}), "Encode -130 fail");
				oxAssert(check(encodeInteger(int64_t(-131)), {255, 125, 255, 255, 255, 255, 255, 255, 255}), "Encode -131 fail");

				oxAssert(check(encodeInteger(uint32_t(0xffffffff)), {0b11101111, 255, 255, 255, 0b00011111}), "Encode 0xffffffff fail");
				oxAssert(check(encodeInteger(uint64_t(1)), {0b0010}), "Encode 1 fail");
				oxAssert(check(encodeInteger(uint64_t(2)), {0b0100}), "Encode 2 fail");
				oxAssert(check(encodeInteger(uint64_t(3)), {0b0110}), "Encode 3 fail");
				oxAssert(check(encodeInteger(uint64_t(4)), {0b1000}), "Encode 4 fail");
				oxAssert(check(encodeInteger(uint64_t(128)), {0b0001, 0b10}), "Encode 128 fail");
				oxAssert(check(encodeInteger(uint64_t(129)), {0b0101, 0b10}), "Encode 129 fail");
				oxAssert(check(encodeInteger(uint64_t(130)), {0b1001, 0b10}), "Encode 130 fail");
				oxAssert(check(encodeInteger(uint64_t(131)), {0b1101, 0b10}), "Encode 131 fail");

				// Signed check needs lambda templates to run correctly without
				// code deduplication
				//oxAssert(check64(encodeInteger(MaxValue<int64_t>), MaxValue<int64_t>), "Encode MaxValue<int64_t> fail");
				oxAssert(check64(encodeInteger(MaxValue<uint64_t>), MaxValue<uint64_t>), "Encode MaxValue<uint64_t> fail");
				return OxError(0);
			}
		},
		{
			"decodeInteger",
			[] {
				using ox::MaxValue;
				using ox::mc::McInt;
				using ox::mc::encodeInteger;
				using ox::mc::decodeInteger;

				constexpr auto check = [](auto val) {
					auto result = decodeInteger<decltype(val)>(encodeInteger(val));
					oxReturnError(result.error);
					if (result.value != val) {
						std::cout << "Bad value: " << result.value << ", expected: " << val << '\n';
						return OxError(1);
					}
					return OxError(0);
				};
				oxAssert(check(uint32_t(14)), "Decode of 14 failed.");
				oxAssert(check(int64_t(-1)), "Decode of -1 failed.");
				oxAssert(check(int64_t(1)), "Decode of 1 failed.");
				oxAssert(check(int64_t(2)), "Decode of 2 failed.");
				oxAssert(check(int64_t(42)), "Decode of 42 failed.");
				oxAssert(check(int64_t(130)), "Decode of 130 failed.");
				oxAssert(check(int64_t(131)), "Decode of 131 failed.");
				oxAssert(check(uint64_t(1)), "Decode of 1 failed.");
				oxAssert(check(uint64_t(2)), "Decode of 2 failed.");
				oxAssert(check(uint64_t(42)), "Decode of 42 failed.");
				oxAssert(check(uint64_t(130)), "Decode of 130 failed.");
				oxAssert(check(uint64_t(131)), "Decode of 131 failed.");
				oxAssert(check(0xffffffff), "Decode of 0xffffffff failed.");
				oxAssert(check(0xffffffffffff), "Decode of 0xffffffffffff failed.");
				oxAssert(check(0xffffffffffffffff), "Decode of U64 max failed.");

				return OxError(0);
			}
		},
		{
			"MetalClawDef",
			[] {
				//constexpr size_t descBuffLen = 1024;
				//uint8_t descBuff[descBuffLen];
				constexpr size_t dataBuffLen = 1024;
				uint8_t dataBuff[dataBuffLen];
				TestStruct testIn, testOut;

				testIn.Bool = true;
				testIn.Int = 42;
				testIn.String = "Test String 1";
				testIn.List[0] = 1;
				testIn.List[1] = 2;
				testIn.List[2] = 3;
				testIn.List[3] = 4;
				testIn.Struct.Bool = false;
				testIn.Struct.Int = 300;
				testIn.Struct.String = "Test String 2";

				oxAssert(ox::writeMC(dataBuff, dataBuffLen, &testIn), "Data generation failed");
				auto type = ox::buildTypeDef(&testIn);
				oxAssert(type.error, "Descriptor write failed");
				ox::walkModel<ox::MetalClawReader>(type.value, dataBuff, dataBuffLen,
					[](const ox::Vector<ox::FieldName>&, const ox::Vector<ox::TypeName>&, const ox::DescriptorField &f, ox::MetalClawReader *rdr) -> ox::Error {
						//std::cout << f.fieldName.c_str() << '\n';
						auto fieldName = f.fieldName.c_str();
						switch (f.type->primitiveType) {
							case ox::PrimitiveType::UnsignedInteger:
								std::cout << fieldName << ":\tuint" << f.type->length * 8 << "_t:\t";
								switch (f.type->length) {
									case 1: {
										uint8_t i = {};
										oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
										std::cout << i;
										break;
									}
									case 2: {
										uint16_t i = {};
										oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
										std::cout << i;
										break;
									}
									case 4: {
										uint32_t i = {};
										oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
										std::cout << i;
										break;
									}
									case 8: {
										uint64_t i = {};
										oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
										std::cout << i;
										break;
									}
								}
								std::cout << '\n';
								break;
							case ox::PrimitiveType::SignedInteger:
								std::cout << fieldName << ":\tint" << f.type->length * 8 << "_t:\t";
								switch (f.type->length) {
									case 1: {
										int8_t i = {};
										oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
										std::cout << i;
										break;
									}
									case 2: {
										int16_t i = {};
										oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
										std::cout << i;
										break;
									}
									case 4: {
										int32_t i = {};
										oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
										std::cout << i;
										break;
									}
									case 8: {
										int64_t i = {};
										oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
										std::cout << i;
										break;
									}
								}
								std::cout << '\n';
								break;
							case ox::PrimitiveType::Bool: {
								bool i = {};
								oxAssert(rdr->field(fieldName, &i), "Walking model failed.");
								std::cout << fieldName << ":\t" << "bool:\t\t" << (i ? "true" : "false") << '\n';
								break;
							}
							case ox::PrimitiveType::String: {
								ox::Vector<char> v(rdr->stringLength(fieldName) + 1);
								//std::cout << rdr->stringLength() << '\n';
								oxAssert(rdr->field(fieldName, ox::SerStr(v.data(), v.size())), "Walking model failed.");
								std::cout << fieldName << ":\t" << "string:\t\t" << v.data() << '\n';
								break;
							}
							case ox::PrimitiveType::Struct:
								break;
							case ox::PrimitiveType::Union:
								break;
						}
						return OxError(0);
					}
				);
				delete type.value;
				return OxError(0);
			}
		},
	}
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 0) {
		auto testName = args[1];
		if (tests.find(testName) != tests.end()) {
			retval = tests[testName]();
		}
	}
	return retval;
}
