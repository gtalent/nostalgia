/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <nostalgia/core/core.hpp>

using namespace nostalgia::core;

int main() {
	Context ctx;
	init(&ctx);
	initConsole(&ctx);
	puts(&ctx, 9 * 32 + 8, "HELLO,WORLD!");
	puts(&ctx, 10 * 32 + 8, "01234 56789");
	while (1);
	return 0;
}
