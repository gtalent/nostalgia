/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../core.hpp"
#include "panic.hpp"

namespace nostalgia {
namespace core {

void panic(const char *msg) {
	initConsole(nullptr);
	puts(nullptr,  1 * 32 + 0, "SADNESS...");
	puts(nullptr,  4 * 32 + 0, "UNEXPECTED STATE:");
	puts(nullptr,  6 * 32 + 2, msg);
	puts(nullptr, 10 * 32 + 0, "PLEASE RESTART THE SYSTEM");
	// TODO: properly end program execution, this wastes power
	while (1);
}

}
}

