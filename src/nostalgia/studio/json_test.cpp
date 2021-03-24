/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <ox/std/assert.hpp>
#include "json_read.hpp"
#include "json_write.hpp"

using namespace nostalgia::studio;

struct TestStructNest {
	bool Bool;
	int Int;
	double Double;
	QString String;
};

template<typename T>
ox::Error model(T *io, TestStructNest *obj) {
	oxReturnError(io->setTypeInfo("TestStructNest", 4));
	oxReturnError(io->field("Bool", &obj->Bool));
	oxReturnError(io->field("Int", &obj->Int));
	oxReturnError(io->field("Double", &obj->Double));
	oxReturnError(io->field("String", &obj->String));
	return OxError(0);
}

struct TestStruct {
	bool Bool;
	int Int;
	double Double;
	QString String;
	TestStructNest Struct;
};

template<typename T>
ox::Error model(T *io, TestStruct *obj) {
	oxReturnError(io->setTypeInfo("TestStruct", 5));
	oxReturnError(io->field("Bool", &obj->Bool));
	oxReturnError(io->field("Int", &obj->Int));
	oxReturnError(io->field("Double", &obj->Double));
	oxReturnError(io->field("String", &obj->String));
	oxReturnError(io->field("Struct", &obj->Struct));
	return OxError(0);
}

int main() {
	auto err = OxError(0);
	QString json;
	TestStruct ts = {
		true,
		42,
		42.42,
		"Test String",
		{
			true,
			42,
			42.42,
			"Test String"
		}
	};
	TestStruct tsOut;
	oxReturnError(writeJson(&json, &ts));
	oxReturnError(readJson(json, &tsOut));

	std::cout << tsOut.Bool << '\n';
	std::cout << tsOut.Int << '\n';
	std::cout << tsOut.Double << '\n';
	std::cout << tsOut.String.toStdString() << '\n';

	oxAssert(tsOut.Bool, "Arg 1 failed");
	oxAssert(tsOut.Int == 42, "Arg 2 failed");
	oxAssert(tsOut.Double == 42.42, "Arg 3 failed");
	oxAssert(tsOut.String == "Test String", "Arg 4 failed");

	oxAssert(tsOut.Struct.Bool, "Arg 5 failed");
	oxAssert(tsOut.Struct.Int == 42, "Arg 6 failed");
	oxAssert(tsOut.Struct.Double == 42.42, "Arg 7 failed");
	oxAssert(tsOut.Struct.String == "Test String", "Arg 8 failed");

	return static_cast<int>(err);
}
