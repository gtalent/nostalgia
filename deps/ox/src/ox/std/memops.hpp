/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "types.hpp"

#if __has_include(<cstring>)
#include<cstring>
#else
extern "C" void *memcpy(void *dest, const void *src, std::size_t size);
#endif

int ox_memcmp(const void *ptr1, const void *ptr2, std::size_t size) noexcept;

constexpr void *ox_memcpy(void *dest, const void *src, std::size_t size) noexcept {
	auto srcBuf = static_cast<const char*>(src);
	auto dstBuf = static_cast<char*>(dest);
	for (std::size_t i = 0; i < size; i++) {
		dstBuf[i] = static_cast<char>(srcBuf[i]);
	}
	return dest;
}

constexpr void *ox_memset(void *ptr, int val, std::size_t size) noexcept {
	auto buf = reinterpret_cast<uint8_t*>(ptr);
	for (std::size_t i = 0; i < size; i++) {
		buf[i] = static_cast<uint8_t>(val);
	}
	return ptr;
}
