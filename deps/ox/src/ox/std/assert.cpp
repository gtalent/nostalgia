/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if defined(OX_USE_STDLIB)
#include <iostream>
#endif

#include "stacktrace.hpp"
#include "trace.hpp"

#include "assert.hpp"

namespace ox {

void assertFunc([[maybe_unused]]const char *file, [[maybe_unused]]int line, [[maybe_unused]]bool pass, [[maybe_unused]]const char *msg) noexcept {
	if (!pass) {
#if defined(OX_USE_STDLIB)
		std::cerr << "\033[31;1;1mASSERT FAILURE:\033[0m (" << file << ':' << line << "): " << msg << std::endl;
		printStackTrace(2);
		oxTrace("assert").del("") << "Failed assert: " << msg << " (" << file << ":" << line << ")";
		std::abort();
#else
		panic(file, line, msg);
#endif
	}
}

void assertFunc(const char *file, int line, const Error &err, const char *assertMsg) noexcept {
	if (err) {
#if defined(OX_USE_STDLIB)
		std::cerr << "\033[31;1;1mASSERT FAILURE:\033[0m (" << file << ':' << line << "): " << assertMsg << '\n';
		if (err.msg) {
			std::cerr <<  "\tError Message:\t" << err.msg << '\n';
		}
		std::cerr <<  "\tError Code:\t" << err << '\n';
		if (err.file != nullptr) {
			std::cerr << "\tError Location:\t" << err.file << ':' << err.line << '\n';
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
void panic(const char *file, int line, const char *panicMsg, const Error &err) noexcept {
	std::cerr << "\033[31;1;1mPANIC:\033[0m (" << file << ':' << line << "): " << panicMsg << '\n';
	if (err.msg) {
		std::cerr <<  "\tError Message:\t" << err.msg << '\n';
	}
	std::cerr <<  "\tError Code:\t" << err << '\n';
	if (err.file != nullptr) {
		std::cerr << "\tError Location:\t" << err.file << ':' << err.line << '\n';
	}
	printStackTrace(2);
	oxTrace("panic").del("") << "Panic: " << panicMsg << " (" << file << ":" << line << ")";
	std::abort();
}
#endif

}
