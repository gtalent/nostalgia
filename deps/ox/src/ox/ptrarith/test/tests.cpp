/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
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
struct OX_PACKED NodeType: public ox::ptrarith::Item<T> {
	public:
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
				using String = BString<6>;
				auto a1 = buffer->malloc(sizeof(String));
				auto a2 = buffer->malloc(sizeof(String));
				oxAssert(a1.valid(), "Allocation 1 failed.");
				oxAssert(a2.valid(), "Allocation 2 failed.");
				auto &s1 = *new (buffer->dataOf<String>(a1)) String("asdf");
				auto &s2 = *new (buffer->dataOf<String>(a2)) String("aoeu");
				oxTrace("test") << "s1: " << s1.c_str();
				oxTrace("test") << "s2: " << s2.c_str();
				oxAssert(s1 == "asdf", "Allocation 1 not as expected.");
				oxAssert(s2 == "aoeu", "Allocation 2 not as expected.");
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
