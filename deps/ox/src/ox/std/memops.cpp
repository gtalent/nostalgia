/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "types.hpp"
#include "memops.hpp"

#ifndef OX_USE_STDLIB

#define ox_inhibit_loop_to_libcall __attribute__((__optimize__("-fno-tree-loop-distribute-patterns")))

extern "C" {

void *ox_inhibit_loop_to_libcall memcpy(void *dest, const void *src, std::size_t size) {
	return ox_memcpy(dest, src, size);
}

void *ox_inhibit_loop_to_libcall memset(void *ptr, int val, std::size_t size) {
	return ox_memset(ptr, val, size);
}

}

#undef ox_inhibit_loop_to_libcall

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
