/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#if defined(OX_USE_STDLIB) && __has_include(<unistd.h>)
#include <iostream>

#include <execinfo.h>
#include <dlfcn.h>
#include <unistd.h>

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#endif
#endif

#include "error.hpp"
#include "string.hpp"
#include "trace.hpp"
#include "units.hpp"
#include "utility.hpp"
#include "vector.hpp"

namespace ox {

#if defined(OX_USE_STDLIB) && __has_include(<unistd.h>)
[[nodiscard]]
static String symbolicate([[maybe_unused]]const char **frames, [[maybe_unused]]std::size_t symbolsLen, [[maybe_unused]]const char *linePrefix) {
#if __has_include(<cxxabi.h>)
	String out;
	for (auto i = 0u; i < symbolsLen; ++i) {
		Dl_info info;
		if (dladdr(frames[i], &info) && info.dli_sname) {
			int status = -1;
			const auto name = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
			if (status == 0) {
				out += sfmt("\t{}: {}\n", i, name);
				continue;
			}
		}
		out += sfmt("\t{}\n", frames[i]);
	}
	return move(out);
#else // __has_include(<cxxabi.h>)
	return {};
#endif // __has_include(<cxxabi.h>)
}
#endif // defined(OX_USE_STDLIB) && __has_include(<unistd.h>)

void printStackTrace([[maybe_unused]]unsigned shave) noexcept {
#if defined(OX_USE_STDLIB) && __has_include(<unistd.h>)
	Vector<void*> frames(1000);
	frames.resize(static_cast<std::size_t>(backtrace(frames.data(), frames.size())));
	if (frames.size() > shave) {
		const auto symbols = backtrace_symbols(frames.data() + shave, frames.size() - shave);
		const auto symbolicatedStacktrace = symbolicate(bit_cast<const char**>(frames.data() + shave), frames.size() - shave, "\t");
		free(symbols);
		oxErrf("Stacktrace:\n{}", symbolicatedStacktrace);
	}
#endif
}

}
