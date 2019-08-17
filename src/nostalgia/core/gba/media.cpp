/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/fs.hpp>
#include <ox/std/std.hpp>
#include "addresses.hpp"

#include "../media.hpp"

namespace nostalgia::core {

uint8_t *loadRom(const char*) {
	// put the header in the wrong order to prevent mistaking this code for the
	// media section
	constexpr auto headerP2 = "_HEADER_________";
	constexpr auto headerP1 = "NOSTALGIA_MEDIA";
	constexpr auto headerP1Len = 15;
	constexpr auto headerP2Len = 16;
	constexpr auto headerLen = headerP1Len + headerP2Len + 1;

	for (auto current = &MEM_ROM; current < ((uint8_t*) 0x0a000000); current += headerLen) {
		if (ox_memcmp(current, headerP1, headerP1Len) == 0 &&
		    ox_memcmp(current + headerP1Len, headerP2, headerP2Len) == 0) {
			return current + headerLen;
		}
	}
	return nullptr;
}

}
