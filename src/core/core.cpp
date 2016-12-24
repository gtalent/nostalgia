/*
 * Copyright 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <ox/std/types.hpp>
#include "gfx.hpp"

namespace nostalgia {
namespace core {

Error init() {
   auto err = initGfx();
	return err;
}

}
}
