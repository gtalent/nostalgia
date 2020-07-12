/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/bit.hpp>
#include <ox/std/heapmgr.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// this warning is too dumb to realize that it can actually confirm the hard
// coded address aligns with the requirement of HeapSegment, so it must be
// suppressed
#pragma GCC diagnostic ignored "-Wcast-align"

#define MEM_EWRAM_BEGIN reinterpret_cast<uint8_t*>(0x02000000)
#define MEM_EWRAM_END   reinterpret_cast<uint8_t*>(0x0203FFFF)

#define HEAP_BEGIN reinterpret_cast<ox::heapmgr::HeapSegment*>(MEM_EWRAM_BEGIN)
// set size to half of EWRAM
#define HEAP_SIZE ((MEM_EWRAM_END - MEM_EWRAM_BEGIN) / 2)
#define HEAP_END  reinterpret_cast<ox::heapmgr::HeapSegment*>(MEM_EWRAM_BEGIN + HEAP_SIZE)

extern void (*__preinit_array_start[]) (void);
extern void (*__preinit_array_end[]) (void);
extern void (*__init_array_start[]) (void);
extern void (*__init_array_end[]) (void);

int main(int argc, const char **argv);

extern "C" {

void __libc_init_array() {
	auto preInits = __preinit_array_end - __preinit_array_start;
	for (decltype(preInits) i = 0; i < preInits; i++) {
		__preinit_array_start[i]();
	}
	auto inits = __init_array_end - __init_array_start;
	for (decltype(inits) i = 0; i < inits; i++) {
		__preinit_array_start[i]();
	}
}

int c_start() {
	const char *args[2] = {"", "rom.oxfs"};
	ox::heapmgr::initHeap(ox::bit_cast<char*>(HEAP_BEGIN), ox::bit_cast<char*>(HEAP_END));
	return main(2, args);
}

}

#pragma GCC diagnostic pop
