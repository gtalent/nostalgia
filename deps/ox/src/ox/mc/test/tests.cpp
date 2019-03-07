/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <assert.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <ox/mc/mc.hpp>
#include <ox/std/std.hpp>

struct TestStructNest {
	bool Bool = false;
	uint32_t Int = 0;
	ox::BString<32> String = "";
};

struct TestStruct {
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
	ox::BString<32> String = "";
	uint32_t List[4] = {0, 0, 0 , 0};
	TestStructNest EmptyStruct;
	TestStructNest Struct;
};

template<typename T>
ox::Error ioOp(T *io, TestStructNest *obj) {
	ox::Error err = 0;
	io->setTypeInfo("TestStructNest", 3);
	err |= io->op("Bool", &obj->Bool);
	err |= io->op("Int", &obj->Int);
	err |= io->op("String", &obj->String);
	return err;
}

template<typename T>
ox::Error ioOp(T *io, TestStruct *obj) {
	ox::Error err = 0;
	io->setTypeInfo("TestStruct", 14);
	err |= io->op("Bool", &obj->Bool);
	err |= io->op("Int", &obj->Int);
	err |= io->op("Int1", &obj->Int1);
	err |= io->op("Int2", &obj->Int2);
	err |= io->op("Int3", &obj->Int3);
	err |= io->op("Int4", &obj->Int4);
	err |= io->op("Int5", &obj->Int5);
	err |= io->op("Int6", &obj->Int6);
	err |= io->op("Int7", &obj->Int7);
	err |= io->op("Int8", &obj->Int8);
	err |= io->op("String", &obj->String);
	err |= io->op("List", obj->List, 4);
	err |= io->op("EmptyStruct", &obj->EmptyStruct);
	err |= io->op("Struct", &obj->Struct);
	return err;
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
				ox::Error err = 0;
				TestStruct ts;

				err |= ox::writeMC(buff, buffLen, &ts);

				return err;
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
				testIn.String = "Test String 1";
				testIn.List[0] = 1;
				testIn.List[1] = 2;
				testIn.List[2] = 3;
				testIn.List[3] = 4;
				testIn.Struct.Bool = false;
				testIn.Struct.Int = 300;
				testIn.Struct.String = "Test String 2";

				oxAssert(ox::writeMC(buff, buffLen, &testIn), "writeMC failed");
				oxAssert(ox::readMC(buff, buffLen, &testOut), "writeMC failed");

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
				oxAssert(testIn.String             == testOut.String, "String value mismatch");
				oxAssert(testIn.List[0]            == testOut.List[0], "List[0] value mismatch");
				oxAssert(testIn.List[1]            == testOut.List[1], "List[1] value mismatch");
				oxAssert(testIn.List[2]            == testOut.List[2], "List[2] value mismatch");
				oxAssert(testIn.List[3]            == testOut.List[3], "List[3] value mismatch");
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
			"MetalClawDef",
			[] {
				ox::Error err = 0;
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
				auto type = ox::buildMCDef(&testIn);
				oxAssert(type.error, "Descriptor write failed");
				ox::walkMC(type.value, dataBuff, dataBuffLen,
					[](const ox::Vector<ox::mc::FieldName>&, const ox::Vector<ox::mc::TypeName>&, const ox::mc::Field &f, ox::MetalClawReader *rdr) -> ox::Error {
						//std::cout << f.fieldName.c_str() << '\n';
						auto fieldName = f.fieldName.c_str();
						switch (f.type->primitiveType) {
							case ox::mc::PrimitiveType::UnsignedInteger:
								std::cout << fieldName << ":\tuint" << f.type->length << "_t:\t";
								switch (f.type->length) {
									case 8: {
										uint8_t i = {};
										oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
										std::cout << i;
										break;
									}
									case 16: {
										uint16_t i = {};
										oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
										std::cout << i;
										break;
									}
									case 32: {
										uint32_t i = {};
										oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
										std::cout << i;
										break;
									}
									case 64: {
										uint64_t i = {};
										oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
										std::cout << i;
										break;
									}
								}
								std::cout << '\n';
								break;
							case ox::mc::PrimitiveType::SignedInteger:
								std::cout << fieldName << ":\tint" << f.type->length << "_t:\t";
								switch (f.type->length) {
									case 8: {
										int8_t i = {};
										oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
										std::cout << i;
										break;
									}
									case 16: {
										int16_t i = {};
										oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
										std::cout << i;
										break;
									}
									case 32: {
										int32_t i = {};
										oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
										std::cout << i;
										break;
									}
									case 64: {
										int64_t i = {};
										oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
										std::cout << i;
										break;
									}
								}
								std::cout << '\n';
								break;
							case ox::mc::PrimitiveType::Bool: {
								bool i = {};
								oxAssert(rdr->op(fieldName, &i), "Walking ioOp failed.");
								std::cout << fieldName << ":\t" << "bool:\t" << (i ? "true" : "false") << '\n';
								break;
							}
							case ox::mc::PrimitiveType::String: {
								ox::Vector<char> v(rdr->stringLength());
								//std::cout << rdr->stringLength() << '\n';
								oxAssert(rdr->op(fieldName, ox::McStr(v.data(), v.size())), "Walking ioOp failed.");
								std::cout << fieldName << ":\t" << "string: " << v.data() << '\n';
								break;
							}
							case ox::mc::PrimitiveType::Struct:
								break;
						}
						return OxError(0);
					}
				);
				delete type.value;

				return err;
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
