/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <map>
#include <string>

#include <ox/std/std.hpp>

namespace ox::heapmgr {

[[nodiscard]] void *malloc(std::size_t allocSize);

void free(void *ptr);

void initHeap(char *heapBegin, char *heapEnd);

}

std::map<std::string, int(*)(std::string)> tests = {
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 1) {
		auto testName = args[1];
		std::string testArg = "";
		if (args[2]) {
			testArg = args[2];
		}
		if (tests.find(testName) != tests.end()) {
			retval = tests[testName](testArg);
		}
	}
	return retval;
}

