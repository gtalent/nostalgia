/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <iostream>
#include <map>
#include <functional>
#include <ox/std/std.hpp>

using namespace std;

map<string, function<int()>> tests = {
	{
		"ABCDEFG != HIJKLMN",
		[]() {
			return !(ox_memcmp("ABCDEFG", "HIJKLMN", 7) < 0);
		}
	},
	{
		"HIJKLMN != ABCDEFG",
		[]() {
			return !(ox_memcmp("HIJKLMN", "ABCDEFG", 7) > 0);
		}
	},
	{
		"ABCDEFG == ABCDEFG",
		[]() {
			return !(ox_memcmp("ABCDEFG", "ABCDEFG", 7) == 0);
		}
	},
	{
		"ABCDEFGHI == ABCDEFG",
		[]() {
			return !(ox_memcmp("ABCDEFGHI", "ABCDEFG", 7) == 0);
		}
	},
};

int main(int argc, const char **args) {
	if (argc > 1) {
		auto testName = args[1];
		if (tests.find(testName) != tests.end()) {
			return tests[testName]();
		}
	}
	return -1;
}
