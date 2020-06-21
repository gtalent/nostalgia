/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "irq.hpp"

namespace nostalgia::core {

constexpr uint16_t DispStat_irq_vblank = 1 << 3;
constexpr uint16_t DispStat_irq_hblank = 1 << 4;
constexpr uint16_t DispStat_irq_vcount = 1 << 5;

void isr();

ox::Error initIrq(Context*) {
	REG_ISR = isr;
	// tell display to trigger vblank interrupts
	REG_DISPSTAT |= DispStat_irq_vblank;
	// tell proc which interrupts to handle
	REG_IE |= Int_vblank;
	REG_IME = 1;
	return OxError(0);
}

}
