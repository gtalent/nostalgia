/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include "json.hpp"

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
	Error err = 0;
	err |= io->setTypeInfo("TestStructNest", 4);
	err |= io->field("Bool", &obj->Bool);
	err |= io->field("Int", &obj->Int);
	err |= io->field("Double", &obj->Double);
	err |= io->field("String", &obj->String);
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
	Error err = 0;
	err |= io->setTypeInfo("TestStruct", 5);
	err |= io->field("Bool", &obj->Bool);
	err |= io->field("Int", &obj->Int);
	err |= io->field("Double", &obj->Double);
	err |= io->field("String", &obj->String);
	err |= io->field("Struct", &obj->Struct);
	return err;
}

int main(int argc, char **args) {
	int err = 0;
	QString json;
	TestStruct ts = {
		.Bool = true,
		.Int = 42,
		.Double = 42.42,
		.String = "Test String",
		.Struct = {
			.Bool = true,
			.Int = 42,
			.Double = 42.42,
			.String = "Test String"
		}
	};
	TestStruct tsOut;
	err |= writeJson(&json, &ts);
	err |= readJson(json, &tsOut);

	cout << tsOut.Bool << endl;
	cout << tsOut.Int << endl;
	cout << tsOut.Double << endl;
	cout << tsOut.String.toStdString() << endl;

	err |= !(tsOut.Bool) << 0;
	err |= !(tsOut.Int == 42) << 1;
	err |= !(tsOut.Double == 42.42) << 2;
	err |= !(tsOut.String == "Test String") << 3;

	err |= !(tsOut.Struct.Bool) << 4;
	err |= !(tsOut.Struct.Int == 42) << 5;
	err |= !(tsOut.Struct.Double == 42.42) << 6;
	err |= !(tsOut.Struct.String == "Test String") << 7;

	return err;
}
