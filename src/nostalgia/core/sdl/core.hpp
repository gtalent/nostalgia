/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <array>

#include <SDL.h>

namespace nostalgia::core {

struct SdlImplData {
	SDL_Window *window = nullptr;
	SDL_GLContext renderer = nullptr;
	std::array<SDL_Texture*, 4> bgTextures{};
};

}
