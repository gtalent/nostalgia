/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "trace.hpp"

namespace ox::trace {

void logError(const char *file, int line, const Error &err) {
	if (err) {
		TraceStream trc(file, line, "ox::error");
		trc <<  "Error:" << err;
		if (err.file != nullptr) {
			trc << "(" << reinterpret_cast<const char*>(err.file) << ":" << err.line << ")";
		}
	}
}

void init() {
	oxTraceInitHook();
}

}
