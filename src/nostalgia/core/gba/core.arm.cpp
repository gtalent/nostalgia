/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <nostalgia/core/config.hpp>
#include <nostalgia/core/core.hpp>

#include "addresses.hpp"
#include "bios.hpp"
#include "irq.hpp"

extern "C" void isr();

namespace nostalgia::core {

extern volatile gba_timer_t g_timerMs;
gba_timer_t g_wakeupTime;
event_handler g_eventHandler = nullptr;

ox::Error run(Context *ctx) {
	g_wakeupTime = 0;
	while (1) {
		if (g_wakeupTime <= g_timerMs && g_eventHandler) {
			auto sleepTime = g_eventHandler(ctx);
			if (sleepTime >= 0) {
				g_wakeupTime = g_timerMs + static_cast<unsigned>(sleepTime);
			} else {
				g_wakeupTime = ~gba_timer_t(0);
			}
		}
		if constexpr(config::GbaEventLoopTimerBased) {
			// wait for timer interrupt
			nostalgia_core_intrwait(0, Int_timer0 | Int_timer1 | Int_timer2 | Int_timer3);
		} else {
			nostalgia_core_vblankintrwait();
		}
	}
	return OxError(0);
}

}
