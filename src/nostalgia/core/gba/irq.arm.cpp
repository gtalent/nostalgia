/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// NOTE: this file is compiled as ARM and not THUMB, so don't but too much in
// here

#include <nostalgia/core/config.hpp>

#include "addresses.hpp"
#include "gfx.hpp"
#include "irq.hpp"

namespace nostalgia::core {

volatile uint16_t g_spriteUpdates = 0;
GbaSpriteAttrUpdate g_spriteBuffer[config::GbaSpriteBufferLen];

}

using namespace nostalgia::core;

extern "C" {

void nostalgia_core_isr_vblank() {
	// copy g_spriteUpdates to allow it to use a register instead of reading
	// from memory every iteration of the loop, needed because g_spriteUpdates
	// is volatile
	const auto updates = g_spriteUpdates;
	for (uint16_t i = 0; i < updates; ++i) {
		auto &oa = g_spriteBuffer[i];
		MEM_OAM[oa.idx] = *reinterpret_cast<uint64_t*>(&oa);
	}
	g_spriteUpdates = 0;
}

void nostalgia_core_isr_timer0() {
}

void nostalgia_core_isr_timer1() {
}

void nostalgia_core_isr_timer2() {
}

void nostalgia_core_isr_timer3() {
}

}
