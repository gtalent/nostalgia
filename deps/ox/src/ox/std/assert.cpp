/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if defined(OX_USE_STDLIB)
#include <bitset>
#include <iostream>
#endif

#include <ox/__buildinfo/defines.hpp>

#include "assert.hpp"

namespace ox {

template<>
void _assert<bool>([[maybe_unused]]const char *file, [[maybe_unused]]int line, [[maybe_unused]]bool pass, [[maybe_unused]]const char *msg) {
#if defined(OX_USE_STDLIB)
	if (!pass) {
		std::cerr << "\033[31;1;1mASSERT FAILURE:\033[0m (" << file << ':' << line << "): " << msg << std::endl;
		std::abort();
	}
#endif
}

template<>
void _assert<Error>([[maybe_unused]]const char *file, [[maybe_unused]]int line, [[maybe_unused]]Error err, [[maybe_unused]]const char *msg) {
#if defined(OX_USE_STDLIB)
	if (err) {
		auto ei = ErrorInfo(err);
		std::cerr << "\033[31;1;1mASSERT FAILURE:\033[0m (" << file << ':' << line << "): " << msg << '\n';
		std::cerr <<  "\tError Code:\t" << ei.errCode << '\n';
		if (ei.file != nullptr) {
			std::cerr << "\tError Location:\t" << reinterpret_cast<const char*>(ei.file) << ':' << ei.line << '\n';
		}
		std::cerr << std::flush;
		std::abort();
	}
#endif
}

}
