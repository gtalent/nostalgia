/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../core.hpp"
#include "panic.hpp"

namespace nostalgia::core {

void panic(const char *msg) {
	oxIgnoreError(initConsole(nullptr));
	puts(nullptr, 32 + 0,  1, "SADNESS...");
	puts(nullptr, 32 + 0,  4, "UNEXPECTED STATE:");
	puts(nullptr, 32 + 2,  6, msg);
	puts(nullptr, 32 + 0, 10, "PLEASE RESTART THE SYSTEM");
	// TODO: properly end program execution, this wastes power
	while (1);
}

}
