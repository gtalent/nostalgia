/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <nostalgia/core/config.hpp>
#include <nostalgia/core/core.hpp>

#include "addresses.hpp"
#include "irq.hpp"

extern "C" void isr();

namespace nostalgia::core {

// Timer Consts
constexpr auto NanoSecond = 1000000000;
constexpr auto MilliSecond = 1000;
constexpr int TicksMs59ns = (NanoSecond / MilliSecond) / 59.59;

extern volatile ox::Uint<config::GbaTimerBits> g_timerMs;

static void initIrq() {
	REG_ISR = isr;
	REG_IME = 1; // enable interrupts
}

static void initTimer() {
	// make timer0 a ~1 millisecond timer
	REG_TIMER0 = TicksMs59ns;
	REG_TIMER0CTL = 0b11000000;
	// enable interrupt for timer0
	REG_IE |= Int_timer0;
}

ox::Error init(Context *ctx) {
	oxReturnError(initGfx(ctx));
	initTimer();
	initIrq();
	return OxError(0);
}

ox::Error run(Context*) {
	while (1) {
	}
	return OxError(0);
}

uint64_t ticksMs() {
	return g_timerMs;
}

}
