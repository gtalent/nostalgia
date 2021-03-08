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

using namespace std;
using namespace ox;
using namespace nostalgia::studio;

struct TestStructNest {
	bool Bool;
	int Int;
	double Double;
	QString String;
};

template<typename T>
Error model(T *io, TestStructNest *obj) {
	auto err = OxError(0);
	oxReturnError(io->setTypeInfo("TestStructNest", 4));
	oxReturnError(io->field("Bool", &obj->Bool));
	oxReturnError(io->field("Int", &obj->Int));
	oxReturnError(io->field("Double", &obj->Double));
	oxReturnError(io->field("String", &obj->String));
	return err;
}

struct TestStruct {
	bool Bool;
	int Int;
	double Double;
	QString String;
	TestStructNest Struct;
};

template<typename T>
Error model(T *io, TestStruct *obj) {
	auto err = OxError(0);
	oxReturnError(io->setTypeInfo("TestStruct", 5));
	oxReturnError(io->field("Bool", &obj->Bool));
	oxReturnError(io->field("Int", &obj->Int));
	oxReturnError(io->field("Double", &obj->Double));
	oxReturnError(io->field("String", &obj->String));
	oxReturnError(io->field("Struct", &obj->Struct));
	return err;
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

	cout << tsOut.Bool << endl;
	cout << tsOut.Int << endl;
	cout << tsOut.Double << endl;
	cout << tsOut.String.toStdString() << endl;

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
