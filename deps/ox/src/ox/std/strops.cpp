/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "strops.hpp"

static_assert(ox_strcmp("asdf", "hijk") < 0, "asdf < hijk");
static_assert(ox_strcmp("hijk", "asdf") > 0, "hijk > asdf");
static_assert(ox_strcmp("resize", "read") > 0, "resize > read");
static_assert(ox_strcmp("read", "resize") < 0, "read < resize");
static_assert(ox_strcmp("resize", "resize") == 0, "resize == resize");
static_assert(ox_strcmp("", "") == 0, "\"\" == \"\"");

static_assert([] {
	auto testStr = "asdf";
	return ox_strchr(testStr, 0, 4) == &testStr[4];
}(), "ox_strchr 0");

static_assert([] {
	int retval = 0;
	auto testStr = "aaaa";
	retval |= !(ox_lastIndexOf((char*) testStr, 'a', ox_strlen(testStr)) == 3);
	retval |= !(ox_lastIndexOf((const char*) testStr, 'a', ox_strlen(testStr)) == 3);
	return retval == 0;
}(), "ox_lastIndexOf aaaa a");
