/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>

#include <ox/__buildinfo/defines.hpp>

namespace ox {

#if defined(OX_USE_STDLIB)
void _assert(const char *file, int line, bool pass, const char *msg) {
	if (!pass) {
		std::cerr << '(' << file << ':' << line << "): " << msg << std::endl;
		std::abort();
	}
}
#else
void _assert(const char*, int, bool, const char*) {
}
#endif

}
