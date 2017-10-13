/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "addresses.hpp"

namespace nostalgia {
namespace core {

static uint8_t *_heapPtr = MEM_WRAM_END;

void clearHeap() {
	_heapPtr = MEM_WRAM_END;
}

}
}

using namespace nostalgia::core;

void *operator new(size_t sz) {
	return _heapPtr -= sz;
}

void operator delete(void *ptr) {
}

