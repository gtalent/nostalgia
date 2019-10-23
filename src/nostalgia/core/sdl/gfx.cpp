/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <SDL.h>

#include "../gfx.hpp"

namespace nostalgia::core {

static SDL_Window *window = nullptr;

ox::Error initGfx(Context*) {
	window = SDL_CreateWindow("nostalgia", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN);
	return OxError(window == nullptr);
}

ox::Error initConsole(Context*) {
	return OxError(1);
}

ox::Error loadTileSheet(Context*, ox::FileAddress) {
	return OxError(1);
}

void puts(Context*, int, const char*) {
}

void setTile(Context*, int, int, int, uint8_t) {
}

}
