/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// NOTE: this file is compiled as ARM and not THUMB, so don't but too much in
// here

#include <ox/std/bit.hpp>

#include "addresses.hpp"
#include "gfx.hpp"
#include "irq.hpp"

namespace nostalgia::core {

volatile uint16_t g_spriteUpdates = 0;
GbaSpriteAttrUpdate g_spriteBuffer[config::GbaSpriteBufferLen];

volatile gba_timer_t g_timerMs = 0;

}

using namespace nostalgia::core;

extern "C" {

void nostalgia_core_isr_vblank() {
	// copy g_spriteUpdates to allow it to use a register instead of reading
	// from memory every iteration of the loop, needed because g_spriteUpdates
	// is volatile
	const unsigned updates = g_spriteUpdates;
	for (unsigned i = 0; i < updates; ++i) {
		const auto &oa = g_spriteBuffer[i];
		MEM_OAM[oa.idx] = *ox::bit_cast<const uint64_t*>(&oa);
	}
	g_spriteUpdates = 0;
	if constexpr(config::GbaEventLoopTimerBased) {
		// disable vblank interrupt until it is needed again
		REG_IE = REG_IE & ~Int_vblank;
	}
}

void nostalgia_core_isr_timer0() {
	g_timerMs = g_timerMs + 1;
}

}
