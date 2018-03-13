/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"
#include "typetraits.hpp"

int ox_strcmp(const char *str1, const char *str2);

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

const char *ox_strchr(const char *str, int character, size_t maxLen = 0xFFFFFFFF);

char *ox_strchr(char *str, int character, size_t maxLen = 0xFFFFFFFF);

int ox_lastIndexOf(const char *str, int character, int maxLen = 0xFFFFFFFF);

int ox_lastIndexOf(char *str, int character, int maxLen = 0xFFFFFFFF);

int ox_atoi(const char *str);

char *ox_itoa(int64_t v, char *str);
