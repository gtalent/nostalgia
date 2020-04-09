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

#include "defines.hpp"
#include "stacktrace.hpp"
#include "trace.hpp"

#include "assert.hpp"

namespace ox {

template<>
void assertFunc<bool>([[maybe_unused]]const char *file, [[maybe_unused]]int line, [[maybe_unused]]bool pass, [[maybe_unused]]const char *msg) {
#if defined(OX_USE_STDLIB)
	if (!pass) {
		std::cerr << "\033[31;1;1mASSERT FAILURE:\033[0m (" << file << ':' << line << "): " << msg << std::endl;
		printStackTrace(2);
		oxTrace("assert").del("") << "Failed assert: " << msg << " (" << file << ":" << line << ")";
		std::abort();
	}
#endif
}

template<>
void assertFunc<Error>(const char *file, int line, Error err, const char *assertMsg) {
	if (err) {
#if defined(OX_USE_STDLIB)
		std::cerr << "\033[31;1;1mASSERT FAILURE:\033[0m (" << file << ':' << line << "): " << assertMsg << '\n';
		if (err.msg) {
			std::cerr <<  "\tError Message:\t" << err.msg << '\n';
		}
		std::cerr <<  "\tError Code:\t" << err << '\n';
		if (err.file != nullptr) {
			std::cerr << "\tError Location:\t" << reinterpret_cast<const char*>(err.file) << ':' << err.line << '\n';
		}
		printStackTrace(2);
		oxTrace("panic").del("") << "Panic: " << assertMsg << " (" << file << ":" << line << ")";
		std::abort();
#else
		panic(file, line, assertMsg);
#endif
	}
}

#if defined(OX_USE_STDLIB)
void panic(const char *file, int line, const char *panicMsg, Error err) {
	std::cerr << "\033[31;1;1mPANIC:\033[0m (" << file << ':' << line << "): " << panicMsg << '\n';
	if (err.msg) {
		std::cerr <<  "\tError Message:\t" << err.msg << '\n';
	}
	std::cerr <<  "\tError Code:\t" << err << '\n';
	if (err.file != nullptr) {
		std::cerr << "\tError Location:\t" << reinterpret_cast<const char*>(err.file) << ':' << err.line << '\n';
	}
	printStackTrace(2);
	oxTrace("panic").del("") << "Panic: " << panicMsg << " (" << file << ":" << line << ")";
	std::abort();
}
#endif

}
