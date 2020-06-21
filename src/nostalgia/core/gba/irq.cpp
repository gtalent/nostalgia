/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "irq.hpp"

namespace nostalgia::core {

constexpr auto DispStat_irq_vblank = static_cast<uint16_t>(1) << 3;
constexpr auto DispStat_irq_hblank = static_cast<uint16_t>(1) << 4;
constexpr auto DispStat_irq_vcount = static_cast<uint16_t>(1) << 5;

void isr();

ox::Error initIrq(Context*) {
	REG_ISR = isr;
	// tell display to trigger vblank interrupts
	REG_DISPSTAT |= DispStat_irq_vblank;
	// tell proc which interrupts to handle
	REG_IE |= IntId_vblank;
	REG_IME = 1;
	return OxError(0);
}

}
