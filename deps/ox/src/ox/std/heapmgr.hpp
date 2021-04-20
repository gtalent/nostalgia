/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "types.hpp"

namespace ox::heapmgr {

struct HeapSegment {
	std::size_t size;
	uint8_t inUse;

	void init(std::size_t maxSize) noexcept;

	template<typename T>
	[[nodiscard]]
	T *data() noexcept;

	template<typename T = uint8_t>
	[[nodiscard]]
	T *end() noexcept;

};

void initHeap(char *heapBegin, char *heapEnd) noexcept;

[[nodiscard]]
void *malloc(std::size_t allocSize) noexcept;

void free(void *ptr) noexcept;

}
