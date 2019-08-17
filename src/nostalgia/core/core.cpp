/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "mem.hpp"
#include "core.hpp"

namespace nostalgia::core {

ox::Error init(Context *ctx) {
	auto err = OxError(0);
	err = initGfx(ctx);
	initHeap(); // this does nothing in userland builds
	return err;
}

}
