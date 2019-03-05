/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "types.hpp"

int ox_memcmp(const void *ptr1, const void *ptr2, std::size_t size) noexcept {
	int retval = 0;
	auto block1 = reinterpret_cast<const uint8_t*>(ptr1);
	auto block2 = reinterpret_cast<const uint8_t*>(ptr2);
	for (std::size_t i = 0; i < size; i++) {
		if (block1[i] < block2[i]) {
			retval = -1;
			break;
		} else if (block1[i] > block2[i]) {
			retval = 1;
			break;
		}
	}
	return retval;
}

void *ox_memcpy(void *dest, const void *src, int64_t size) noexcept {
	auto srcBuf = static_cast<const char*>(src);
	auto dstBuf = static_cast<char*>(dest);
	for (int64_t i = 0; i < size; i++) {
		dstBuf[i] = static_cast<char>(srcBuf[i]);
	}
	return dest;
}

void *ox_memset(void *ptr, int val, int64_t size) noexcept {
	auto buf = static_cast<char*>(ptr);
	for (int64_t i = 0; i < size; i++) {
		buf[i] = val;
	}
	return ptr;
}
