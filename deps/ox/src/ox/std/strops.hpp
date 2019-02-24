/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "math.hpp"
#include "types.hpp"
#include "typetraits.hpp"

template<typename T1, typename T2>
constexpr char *ox_strncpy(T1 dest, T2 src, std::size_t maxLen) noexcept {
	for (std::size_t i = 0; i < maxLen && src[i]; i++) {
		dest[i] = src[i];
	}
	return dest;
}

[[nodiscard]] constexpr int ox_strnlen(const char *str1, int maxLen) noexcept {
	int len = 0;
	for (; len < maxLen && str1[len]; len++);
	return len;
}

template<typename T>
[[nodiscard]] constexpr int ox_strlen(T str1) noexcept {
	int len = 0;
	for (; str1[len]; len++);
	return len;
}

template<typename T1, typename T2>
[[nodiscard]] constexpr int ox_strcmp(T1 str1, T2 str2) noexcept {
	auto retval = 0;
	auto i = 0;
	while (str1[i] || str2[i]) {
		if (str1[i] < str2[i]) {
			retval = -1;
			break;
		} else if (str1[i] > str2[i]) {
			retval = 1;
			break;
		}
		i++;
	}
	return retval;
}

[[nodiscard]] constexpr int ox_strncmp(const char *str1, const char *str2, std::size_t len) noexcept {
	auto retval = 0;
	std::size_t i = 0;
	while (i < len && (str1[i] || str2[i])) {
		if (str1[i] < str2[i]) {
			retval = -1;
			break;
		} else if (str1[i] > str2[i]) {
			retval = 1;
			break;
		}
		i++;
	}
	return retval;
}

[[nodiscard]] constexpr const char *ox_strchr(const char *str, int character, std::size_t maxLen = 0xFFFFFFFF) noexcept {
	for (std::size_t i = 0; i <= maxLen; i++) {
		if (str[i] == character) {
			return &str[i];
		} else if (str[i] == 0) {
			return nullptr;
		}
	}
	return nullptr;
}

[[nodiscard]] constexpr char *ox_strchr(char *str, int character, std::size_t maxLen = 0xFFFFFFFF) noexcept {
	for (std::size_t i = 0; i < maxLen; i++) {
		if (str[i] == character) {
			return &str[i];
		} else if (str[i] == 0) {
			return nullptr;
		}
	}
	return nullptr;
}

[[nodiscard]] constexpr int ox_lastIndexOf(const char *str, int character, int maxLen = 0xFFFFFFFF) noexcept {
	int retval = -1;
	for (int i = 0; i < maxLen && str[i]; i++) {
		if (str[i] == character) {
			retval = i;
		}
	}
	return retval;
}

[[nodiscard]] constexpr int ox_lastIndexOf(char *str, int character, int maxLen = 0xFFFFFFFF) noexcept {
	int retval = -1;
	for (int i = 0; i < maxLen && str[i]; i++) {
		if (str[i] == character) {
			retval = i;
		}
	}
	return retval;
}

[[nodiscard]] constexpr int ox_atoi(const char *str) noexcept {
	int total = 0;
	int multiplier = 1;

	for (auto i = ox_strlen(str) - 1; i != -1; i--) {
		total += (str[i] - '0') * multiplier;
		multiplier *= 10;
	}

	return total;
}

template<typename T>
constexpr T ox_itoa(int64_t v, T str) noexcept {
	if (v) {
		auto mod = 1000000000000000000;
		constexpr auto base = 10;
		auto it = 0;
		if (v < 0) {
			str[it] = '-';
			it++;
		}
		while (mod) {
			auto digit = v / mod;
			v %= mod;
			mod /= base;
			if (it or digit) {
				int start = '0';
				if (digit >= 10) {
					start = 'a';
					digit -= 10;
				}
				str[it] = start + digit;
				it++;
			}
		}
		str[it] = 0;
	} else {
		// 0 is a special case
		str[0] = '0';
		str[1] = 0;
	}
	return str;
}
