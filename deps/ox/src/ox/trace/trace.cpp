/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdarg.h>
#include <stdio.h>

#include <ox/std/std.hpp>

#include "trace.hpp"

namespace ox {

struct TraceMsg {
	const char *file;
	int line;
	uint64_t time;
	const char *ch;
	const char *msg;
};

void trace(const char *file, int line, const char *ch, const char *msg) {
}

}
