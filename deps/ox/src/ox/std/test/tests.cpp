/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#undef NDEBUG

#include <iostream>
#include <map>
#include <functional>
#include <ox/std/std.hpp>

std::map<std::string, std::function<ox::Error()>> tests = {
	{
		"malloc",
		[] {
			ox::Vector<char> buff(ox::units::MB);
			ox::heapmgr::initHeap(&buff[0], &buff[buff.size()-1]);
			oxAssert(ox::heapmgr::malloc(5) != nullptr, "malloc is broken");
			oxAssert(ox::heapmgr::malloc(5) != nullptr, "malloc is broken");
			return OxError(0);
		}
	},
	{
		"ABCDEFG != HIJKLMN",
		[]() {
			return OxError(ox_memcmp("ABCDEFG", "HIJKLMN", 7) >= 0);
		}
	},
	{
		"HIJKLMN != ABCDEFG",
		[]() {
			return OxError(ox_memcmp("HIJKLMN", "ABCDEFG", 7) <= 0);
		}
	},
	{
		"ABCDEFG == ABCDEFG",
		[]() {
			return OxError(ox_memcmp("ABCDEFG", "ABCDEFG", 7) != 0);
		}
	},
	{
		"ABCDEFGHI == ABCDEFG",
		[]() {
			return OxError(ox_memcmp("ABCDEFGHI", "ABCDEFG", 7) != 0);
		}
	},
	{
		"BString",
		[]() {
			ox::BString<5> s;
			s += "A";
			s += "B";
			s += 9;
			s += "C";
			oxAssert(s == "AB9C", "BString append broken");
			s = "asdf";
			oxAssert(s == "asdf", "String assign broken");
			return OxError(0);
		}
	},
	{
		"String",
		[]() {
			ox::String s;
			s += "A";
			s += "B";
			s += 9;
			s += "C";
			oxAssert(s == "AB9C", "String append broken");
			s = "asdf";
			oxAssert(s == "asdf", "String assign broken");
			s += "aoeu";
			oxAssert(s == "asdfaoeu", "String append broken");
			ox::String ending = "asdf";
			oxAssert(ending.endsWith("df"), "String::endsWith is broken");
			oxAssert(!ending.endsWith("awefawe"), "String::endsWith is broken");
			oxAssert(!ending.endsWith("eu"), "String::endsWith is broken");
			return OxError(0);
		}
	},
	{
		"Vector",
		[] {
			ox::Vector<int> v;
			oxAssert(v.size() == 0, "Initial Vector size not 0");
			auto insertTest = [&v](int val, [[maybe_unused]] std::size_t size) {
				v.push_back(val);
				oxAssert(v.size() == size, "Vector size incorrect");
				oxAssert(v[v.size() - 1] == val, "Vector value wrong");
			};
			insertTest(42, 1);
			insertTest(100, 2);
			insertTest(102, 3);
			return OxError(0);
		}
	},
	{
		"HashMap",
		[] {
			ox::HashMap<const char*, int> v;
			v["asdf"] = 42;
			v["aoeu"] = 100;
			oxAssert(v["asdf"] == 42, "asdf != 42");
			oxAssert(v["aoeu"] == 100, "aoeu != 100");
			return OxError(0);
		}
	},
};

int main(int argc, const char **args) {
	if (argc > 1) {
		auto testName = args[1];
		if (tests.find(testName) != tests.end()) {
			oxAssert(tests[testName](), "Test returned Error");
			return 0;
		}
	}
	return -1;
}
