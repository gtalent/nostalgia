/*
 * Copyright 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "registers.hpp"
#include "../types.hpp"

namespace nostalgia {
namespace core {

Error initGfx() {
	/* Sprite Mode ----\ */
	/*             ---\| */
	/* Background 2 -\|| */
	/* Objects -----\||| */
	/*              |||| */
	REG_DISPCNT = 0x1400;

	return 0;
}

}
}
