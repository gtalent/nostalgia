/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "../gfx.hpp"

namespace nostalgia::core {

ox::Error initGfx(Context*) noexcept {
	return OxError(1);
}

ox::Error shutdownGfx(Context*) noexcept {
	return OxError(1);
}

ox::Error initConsole(Context*) noexcept {
	return OxError(1);
}

ox::Error loadBgTileSheet(Context*,
                          int,
                          ox::FileAddress,
                          ox::FileAddress) noexcept {
	return OxError(1);
}

void puts(Context *ctx, int column, int row, const char *str) noexcept {
	for (int i = 0; str[i]; i++) {
		setTile(ctx, 0, column + i, row, static_cast<uint8_t>(charMap[static_cast<int>(str[i])]));
	}
}

void setTile(Context*, int, int, int, uint8_t) noexcept {
}

void setSprite(Context*, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned) noexcept {
}

}
