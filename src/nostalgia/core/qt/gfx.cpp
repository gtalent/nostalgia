/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../gfx.hpp"

namespace nostalgia::core {

ox::Error initGfx(Context*) {
	return OxError(1);
}

ox::Error shutdownGfx(Context*) {
	return OxError(1);
}

ox::Error initConsole(Context*) {
	return OxError(1);
}

ox::Error loadBgTileSheet(Context*,
                          int,
                          ox::FileAddress,
                          ox::FileAddress) {
	return OxError(1);
}

void setTile(Context*, int, int, int, uint8_t) {
}

}
