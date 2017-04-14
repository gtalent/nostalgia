/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "core/core.hpp"

using namespace nostalgia;

int main() {
	core::init();
	core::initConsole();
	core::puts(296, "HELLO, WORLD!");
	while (1);
	return 0;
}
