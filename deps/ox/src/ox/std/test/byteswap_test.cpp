/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <map>
#include <string>
#include <ox/std/std.hpp>

using namespace std;
using namespace ox;

template<typename T>
int testBigEndianAdapt(string str) {
	auto i = (T) stoull(str, nullptr, 16);
	return !(bigEndianAdapt(bigEndianAdapt(i)) == i);
}

map<string, int(*)(string)> tests = {
	{
		{ "bigEndianAdapt<uint16_t>", testBigEndianAdapt<uint16_t> },
		{ "bigEndianAdapt<uint32_t>", testBigEndianAdapt<uint32_t> },
		{ "bigEndianAdapt<uint64_t>", testBigEndianAdapt<uint64_t> },
	},
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 1) {
		auto testName = args[1];
		string testArg = args[2];
		if (tests.find(testName) != tests.end()) {
			retval = tests[testName](testArg);
		}
	}
	return retval;
}
