/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#undef NDEBUG

#include <assert.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <ox/claw/format.hpp>
#include <ox/claw/read.hpp>
#include <ox/claw/write.hpp>
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
	static constexpr auto Fields = 16;
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
	oxReturnError(io->field("EmptyStruct", &obj->EmptyStruct));
	oxReturnError(io->field("Struct", &obj->Struct));
	return OxError(0);
}

std::map<std::string, ox::Error(*)()> tests = {
	{
		{
			"ClawHeaderReader",
			[] {
				ox::String hdr = "O1;com.drinkingtea.ox.claw.test.Header;2;";
				auto [ch, err] = ox::detail::readHeader(hdr.c_str(), hdr.len() + 1);
				oxAssert(err, "Error parsing header");
				oxAssert(ch.fmt == ox::ClawFormat::Organic, "Format wrong");
				oxAssert(ch.typeName == "com.drinkingtea.ox.claw.test.Header", "Type name wrong");
				oxAssert(ch.typeVersion == 2, "Type version wrong");
				return OxError(0);
			}
		},
		{
			"ClawHeaderReader2",
			[] {
				ox::String hdr = "M1;com.drinkingtea.ox.claw.test.Header2;3;";
				auto [ch, err] = ox::detail::readHeader(hdr.c_str(), hdr.len() + 1);
				oxAssert(err, "Error parsing header");
				oxAssert(ch.fmt == ox::ClawFormat::Metal, "Format wrong");
				oxAssert(ch.typeName == "com.drinkingtea.ox.claw.test.Header2", "Type name wrong");
				oxAssert(ch.typeVersion == 3, "Type version wrong");
				return OxError(0);
			}
		},
		{
			"ClawWriter",
			[] {
				// This test doesn't confirm much, but it does show that the writer
				// doesn't segfault
				TestStruct ts;
				oxReturnError(ox::writeClaw(&ts, ox::ClawFormat::Metal));
				return OxError(0);
			}
		},
		{
			"ClawReader",
			[] {
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
				testIn.Struct.Bool = false;
				testIn.Struct.Int = 300;
				testIn.Struct.String = "Test String 2";

				auto [buff, err] = ox::writeClaw(&testIn, ox::ClawFormat::Metal);
				oxAssert(err, "writeMC failed");
				oxAssert(ox::readClaw(buff.data(), buff.size(), &testOut), "writeMC failed");
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
				oxAssert(testIn.EmptyStruct.Bool   == testOut.EmptyStruct.Bool, "EmptyStruct.Bool value mismatch");
				oxAssert(testIn.EmptyStruct.Int    == testOut.EmptyStruct.Int, "EmptyStruct.Int value mismatch");
				oxAssert(testIn.EmptyStruct.String == testOut.EmptyStruct.String, "EmptyStruct.String value mismatch");
				oxAssert(testIn.Struct.Int         == testOut.Struct.Int, "Struct.Int value mismatch");
				oxAssert(testIn.Struct.String      == testOut.Struct.String, "Struct.String value mismatch");
				oxAssert(testIn.Struct.Bool        == testOut.Struct.Bool, "Struct.Bool value mismatch");

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
		} else {
			retval = 1;
		}
	}
	return retval;
}
