/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

// Functions for accessing BIOS calls

extern "C" {

// waits for any interrupt
void nostalgia_core_halt();

void nostalgia_core_stop();

// waits for interrupts specified in interSubs
void nostalgia_core_intrwait(unsigned discardExistingIntrs, unsigned intrSubs);

// waits for vblank interrupt
void nostalgia_core_vblankintrwait();

}
