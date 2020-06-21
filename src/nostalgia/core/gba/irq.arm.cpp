/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// NOTE: this file is compiled as ARM and not THUMB, so don't but too much in
// here

#include "irq.hpp"

namespace nostalgia::core {

void isr() {
	REG_IF = Int_vblank;
}

}
