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

#include "assert.hpp"
#include "bstring.hpp"
#include "string.hpp"
#include "strops.hpp"
#include "types.hpp"
#include "typetraits.hpp"

namespace ox {
namespace detail {

constexpr const char *toCString(const char *s) noexcept {
	return s;
}

template<std::size_t size>
constexpr const char *toCString(const BString<size> &s) noexcept {
	return s.c_str();
}

template<std::size_t size>
constexpr const char *toCString(const BasicString<size> &s) noexcept {
	return s.c_str();
}

#if __has_include(<string>)
#if __cplusplus >= 202002L
constexpr
#else
inline
#endif
const char *toCString(const std::string &s) noexcept {
	return s.c_str();
}
#endif

#if __has_include(<QString>)
inline const char *toCString(const QString &s) noexcept {
	return s.toUtf8();
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
				out = toCString(v);
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

}

template<typename StringType = String, typename ...Args>
[[nodiscard]]
StringType sfmt(const char *fmt, Args... args) noexcept {
	oxAssert(ox::detail::argCount(fmt) == sizeof...(args), "Argument count mismatch.");
	StringType out;
	const auto fmtSegments = ox::detail::fmtSegments<sizeof...(args)+1>(fmt);
	const auto &firstSegment = fmtSegments.segments[0];
	oxIgnoreError(out.append(firstSegment.str, firstSegment.length));
	const detail::FmtArg elements[sizeof...(args)] = {args...};
	for (auto i = 0u; i < fmtSegments.size - 1; ++i) {
		out += elements[i].out;
		const auto &s = fmtSegments.segments[i + 1];
		oxIgnoreError(out.append(s.str, s.length));
	}
	return move(out);
}

}
