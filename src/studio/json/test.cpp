/*
 * Copyright 2016-2017 gtalent2@gmail.com
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
int ioOp(T *io, TestStructNest *obj) {
	Error err = 0;
	err |= io->op("Bool", &obj->Bool);
	err |= io->op("Int", &obj->Int);
	err |= io->op("Double", &obj->Double);
	err |= io->op("String", &obj->String);
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
int ioOp(T *io, TestStruct *obj) {
	Error err = 0;
	err |= io->op("Bool", &obj->Bool);
	err |= io->op("Int", &obj->Int);
	err |= io->op("Double", &obj->Double);
	err |= io->op("String", &obj->String);
	err |= io->op("Struct", &obj->Struct);
	return err;
}

int main(int argc, char **args) {
	int err = 0;
	auto json = 
		"{"
		"	\"Bool\": true,"
		"	\"Int\": 42,"
		"	\"Double\": 42.42,"
		"	\"String\": \"Test String\","
		"	\"Struct\": {"
		"		\"Bool\": true,"
		"		\"Int\": 42,"
		"		\"Double\": 42.42,"
		"		\"String\": \"Test String\""
		"	}"
		"}";
	TestStruct ts;
	read(json, &ts);

	cout << ts.Bool << endl;
	cout << ts.Int << endl;
	cout << ts.Double << endl;
	cout << ts.String.toStdString() << endl;

	err |= !(ts.Bool) << 0;
	err |= !(ts.Int == 42) << 1;
	err |= !(ts.Double == 42.42) << 2;
	err |= !(ts.String == "Test String") << 3;

	err |= !(ts.Struct.Bool) << 4;
	err |= !(ts.Struct.Int == 42) << 5;
	err |= !(ts.Struct.Double == 42.42) << 6;
	err |= !(ts.Struct.String == "Test String") << 7;

	return err;
}
