/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if defined(OX_USE_STDLIB)
#include <array>
#include <execinfo.h>
#include <iostream>
#include <unistd.h>
#endif

#include "stacktrace.hpp"

namespace ox {

void printStackTrace([[maybe_unused]]int shave) {
#if defined(OX_USE_STDLIB)
	std::array<void*, 100> frames;
	auto size = static_cast<int>(backtrace(frames.data(), frames.size()));
	if (size > shave) {
		std::cout << "\nStacktrace:\n";
		backtrace_symbols_fd(frames.data() + shave, size - shave, STDERR_FILENO);
	}
#endif
}

}
