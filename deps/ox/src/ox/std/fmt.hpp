/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#if __has_include(<string>)
#include <string>
#endif

#if __has_include(<QString>)
#include <QString>
#endif

#include <ox/std/bstring.hpp>
#include <ox/std/string.hpp>
#include <ox/std/strops.hpp>
#include <ox/std/types.hpp>
#include <ox/std/typetraits.hpp>

namespace ox::detail {

constexpr const char *stringify(const char *s) noexcept {
	return s;
}

template<std::size_t size>
constexpr const char *stringify(const BString<size> &s) noexcept {
    return s.c_str();
}

constexpr const char *stringify(const String &s) noexcept {
	return s.c_str();
}

#if __has_include(<string>) && __cplusplus >= 202002L
constexpr const char *stringify(const std::string &s) noexcept {
	return s.c_str();
}
#endif

class FmtArg {

	private:
		char dataStr[10] = {};

	public:
		const char *out = nullptr;

		template<typename T>
		constexpr FmtArg(const T &v) noexcept {
			if constexpr(is_bool_v<T>) {
				out = v ? "true" : "false";
			} else if constexpr(is_integral_v<T>) {
				out = ox_itoa(v, dataStr);
			} else {
				out = stringify(v);
			}
		}

};

[[nodiscard]]
constexpr uint64_t argCount(const char *str) noexcept {
	uint64_t cnt = 0;
	const auto prev = [str](std::size_t i) -> char {
		if (i > 0) {
			return str[i - 1];
		} else {
			return '\0';
		}
	};
	const auto next = [str](std::size_t i) -> char {
		if (i < ox_strlen(str) - 1) {
			return str[i + 1];
		} else {
			return '\0';
		}
	};
	for (std::size_t i = 0; i < ox_strlen(str); ++i) {
		if (str[i] == '{' && prev(i) != '\\' && next(i) == '}') {
			++cnt;
		}
	}
	return cnt;
}

static_assert(argCount("sadf asdf") == 0);
static_assert(argCount("{}") == 1);
static_assert(argCount("{}{}") == 2);
static_assert(argCount("thing1: {}, thing2: {}") == 2);
static_assert(argCount("thing1: {}, thing2: {}{}") == 3);

struct FmtSegment {
	const char *str = nullptr;
	unsigned length = 0;

	constexpr bool operator==(const FmtSegment &o) const noexcept {
		return length == o.length && ox_strncmp(str, o.str, length) == 0;
	}

	constexpr bool operator!=(const FmtSegment &o) const noexcept {
		return length != o.length || ox_strncmp(str, o.str, length) != 0;
	}
};

template<std::size_t sz>
struct Fmt {
	static constexpr std::size_t size = sz;
	FmtSegment segments[sz];

	constexpr bool operator==(const Fmt<sz> &o) const noexcept {
		for (std::size_t i = 0; i < sz; ++i) {
			if (segments[i] != o.segments[i]) {
				return false;
			}
		}
		return true;
	}
};

template<std::size_t segementCnt>
[[nodiscard]]
constexpr Fmt<segementCnt> fmtSegments(const char *fmt) noexcept {
	Fmt<segementCnt> out;
	const auto prev = [fmt](std::size_t i) -> char {
		if (i > 0) {
			return fmt[i - 1];
		} else {
			return '\0';
		}
	};
	const auto next = [fmt](std::size_t i) -> char {
		if (i < ox_strlen(fmt) - 1) {
			return fmt[i + 1];
		} else {
			return '\0';
		}
	};
	auto current = &out.segments[0];
	current->str = fmt;
	for (std::size_t i = 0; i < ox_strlen(fmt); ++i) {
		if (fmt[i] == '{' && prev(i) != '\\' && next(i) == '}') {
			++current;
			current->str = fmt + i + 2;
			current->length = 0;
			i += 1;
		} else {
			++current->length;
		}
	}
	return out;
}

static_assert([] {
		constexpr auto fmt = "{}";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<2>{
			{
				{"", 0},
				{"", 0},
			}
		};
	}()
);
static_assert([] {
		constexpr auto fmt = "thing: {}";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<2>{
			{
				{"thing: ", 7},
				{"", 0},
			}
		};
	}()
);
static_assert([] {
		constexpr auto fmt = "thing: {}, thing2: {}";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<3>{
			{
				{"thing: ", 7},
				{", thing2: ", 10},
				{"", 0},
			}
		};
	}()
);
static_assert([] {
		constexpr auto fmt = "thing: {}, thing2: {}s";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<3>{
			{
				{"thing: ", 7},
				{", thing2: ", 10},
				{"s", 1},
			}
		};
	}()
);
static_assert([] {
		constexpr auto fmt = "loadTexture: section: {}, w: {}, h: {}";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<4>{
			{
				{"loadTexture: section: ", 22},
				{", w: ", 5},
				{", h: ", 5},
			}
		};
	}()
);


}
