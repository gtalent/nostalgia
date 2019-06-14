/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "types.hpp"
#include "memops.hpp"

#ifndef OX_USE_STDLIB

extern "C" {

void *memcpy(void *dest, const void *src, std::size_t size) {
	return ox_memcpy(dest, src, size);
}

void *memset(void *ptr, int val, std::size_t size) {
	return ox_memset(ptr, val, size);
}

}

#endif

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
