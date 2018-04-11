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

constexpr int ox_strlen(const char *str1) {
	int len = 0;
	for (; str1[len]; len++);
	return len;
}

constexpr int ox_strlen(char *str1) {
	int len = 0;
	for (; str1[len]; len++);
	return len;
}

constexpr int ox_strcmp(const char *str1, const char *str2) {
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

constexpr const char *ox_strchr(const char *str, int character, size_t maxLen = 0xFFFFFFFF) {
	for (size_t i = 0; i <= maxLen; i++) {
		if (str[i] == character) {
			return &str[i];
		} else if (str[i] == 0) {
			return nullptr;
		}
	}
	return nullptr;
}

constexpr char *ox_strchr(char *str, int character, size_t maxLen = 0xFFFFFFFF) {
	for (size_t i = 0; i < maxLen; i++) {
		if (str[i] == character) {
			return &str[i];
		} else if (str[i] == 0) {
			return nullptr;
		}
	}
	return nullptr;
}

constexpr int ox_lastIndexOf(const char *str, int character, int maxLen = 0xFFFFFFFF) {
	int retval = -1;
	for (int i = 0; i < maxLen && str[i]; i++) {
		if (str[i] == character) {
			retval = i;
		}
	}
	return retval;
}

constexpr int ox_lastIndexOf(char *str, int character, int maxLen = 0xFFFFFFFF) {
	int retval = -1;
	for (int i = 0; i < maxLen && str[i]; i++) {
		if (str[i] == character) {
			retval = i;
		}
	}
	return retval;
}

constexpr int ox_atoi(const char *str) {
	int total = 0;
	int multiplier = 1;

	for (auto i = ox_strlen(str) - 1; i != -1; i--) {
		total += (str[i] - '0') * multiplier;
		multiplier *= 10;
	}

	return total;
}

constexpr char *ox_itoa(int64_t v, char *str) {
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
