/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if defined(OX_USE_STDLIB)
#include <iomanip>
#include <iostream>

static const auto OxPrintTrace = std::getenv("OXTRACE") != nullptr;
#else
constexpr auto OxPrintTrace = false;
#endif

extern "C" {

void oxTraceInitHook() {
}

void oxTraceHook([[maybe_unused]] const char *file, [[maybe_unused]] int line,
                 [[maybe_unused]] const char *ch, [[maybe_unused]] const char *msg) {
#if defined(OX_USE_STDLIB)
	if (OxPrintTrace) {
		std::cout << std::setw(53) << std::left << ch << "| ";
		std::cout << std::setw(65) << std::left << msg << '|';
		std::cout << " " << file << ':' << line << "\n";
	}
#endif
}

}

