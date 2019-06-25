/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// make sure asserts are enabled for the test file
#undef NDEBUG

#include <iostream>
#include <assert.h>
#include <array>
#include <map>
#include <vector>
#include <string>
#include <ox/ptrarith/nodebuffer.hpp>
#include <ox/fs/fs.hpp>
#include <ox/std/std.hpp>
#include <ox/fs/filestore/filestoretemplate.hpp>
#include <ox/fs/filesystem/filesystem.hpp>


using namespace std;
using namespace ox;

template<typename T>
struct __attribute__((packed)) NodeType: public ox::ptrarith::Item<T> {
	public:
		int i = 0;
		size_t fullSize() const {
			return this->size() + sizeof(*this);
		}
};

map<string, int(*)(string)> tests = {
	{
		{
			"PtrArith::setSize",
			[](string) {
				using BuffPtr_t = uint32_t;
				std::vector<char> buff(5 * ox::units::MB);
				auto buffer = new (buff.data()) ox::ptrarith::NodeBuffer<BuffPtr_t, NodeType<BuffPtr_t>>(buff.size());
				auto a1 = buffer->malloc(50);
				auto a2 = buffer->malloc(50);
				oxAssert(buffer->free(a1), "Free failed.");
				oxAssert(buffer->free(a2), "Free failed.");
				oxAssert(buffer->setSize(buffer->size() - buffer->available()), "Resize failed.");
				return 0;
			}
		},
	},
};

int main(int argc, const char **args) {
	int retval = -1;
	if (argc > 1) {
		auto testName = args[1];
		string testArg = "";
		if (args[2]) {
			testArg = args[2];
		}
		if (tests.find(testName) != tests.end()) {
			retval = tests[testName](testArg);
		}
	}
	return retval;
}
