/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/__buildinfo/defines.hpp>

#if defined(OX_USE_STDLIB)

#include <iostream>

#endif

namespace ox {

void _assert([[maybe_unused]]const char *file, [[maybe_unused]]int line, [[maybe_unused]]bool pass, [[maybe_unused]]const char *msg) {
#if defined(OX_USE_STDLIB)
	if (!pass) {
		std::cerr << '(' << file << ':' << line << "): " << msg << std::endl;
		std::abort();
	}
#endif
}

}
