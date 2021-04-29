/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <nostalgia/core/config.hpp>
#include <nostalgia/core/core.hpp>
#include <nostalgia/core/input.hpp>

#include "addresses.hpp"
#include "bios.hpp"
#include "irq.hpp"

extern "C" void isr();

namespace nostalgia::core {

// Timer Consts
constexpr int NanoSecond = 1000000000;
constexpr int MilliSecond = 1000;
constexpr int TicksMs59ns = 65535 - (NanoSecond / MilliSecond) / 59.59;

extern event_handler g_eventHandler;

extern volatile gba_timer_t g_timerMs;

static void initIrq() noexcept {
	REG_ISR = isr;
	REG_IME = 1; // enable interrupts
}

static void initTimer() noexcept {
	// make timer0 a ~1 millisecond timer
	REG_TIMER0 = TicksMs59ns;
	REG_TIMER0CTL = 0b11000000;
	// enable interrupt for timer0
	REG_IE = REG_IE | Int_timer0;
}

ox::Error init(Context *ctx) noexcept {
	oxReturnError(initGfx(ctx));
	initTimer();
	initIrq();
	return OxError(0);
}

void setEventHandler(Context*, event_handler h) noexcept {
	g_eventHandler = h;
}

uint64_t ticksMs(Context*) noexcept {
	return g_timerMs;
}

bool buttonDown(Key k) noexcept {
	return !(REG_GAMEPAD & k);
}

}
