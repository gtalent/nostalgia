/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "nostalgia/common/point.hpp"
#include <array>
#ifdef NOST_FPS_PRINT
#include <iostream>
#endif
#include <iostream>

#include <SDL.h>

#include <ox/claw/read.hpp>

#include <nostalgia/core/gfx.hpp>
#include <nostalgia/core/userland/gfx.hpp>

#include "core.hpp"

namespace nostalgia::core {

constexpr auto Scale = 5;

ox::Error initGfx(Context *ctx) {
	auto id = new SdlImplData;
	ctx->setWindowerData(id);
	id->window = SDL_CreateWindow("nostalgia", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	                              240 * Scale, 160 * Scale,
	                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	if (id->window == nullptr) {
		return OxError(1, SDL_GetError());
	}
	id->renderer = SDL_GL_CreateContext(id->window);
	if (id->renderer == nullptr) {
		return OxError(1, SDL_GetError());
	}
	oxReturnError(renderer::init(ctx));
	return OxError(0);
}

ox::Error shutdownGfx(Context *ctx) {
	oxReturnError(renderer::shutdown(ctx));
	auto id = ctx->windowerData<SdlImplData>();
	for (auto tex : id->bgTextures) {
		if (tex) {
			SDL_DestroyTexture(tex);
		}
	}
	SDL_GL_DeleteContext(id->renderer);
	SDL_DestroyWindow(id->window);
	ctx->setWindowerData(nullptr);
	delete id;
	return OxError(0);
}

int getScreenWidth(Context *ctx) {
	auto id = ctx->windowerData<SdlImplData>();
	int x = 0, y = 0;
	SDL_GetWindowSize(id->window, &x, &y);
	return x;
}

int getScreenHeight(Context *ctx) {
	auto id = ctx->windowerData<SdlImplData>();
	int x = 0, y = 0;
	SDL_GetWindowSize(id->window, &x, &y);
	return y;
}

common::Point getScreenSize(Context *ctx) {
	auto id = ctx->windowerData<SdlImplData>();
	int x = 0, y = 0;
	SDL_GetWindowSize(id->window, &x, &y);
	return {x, y};
}

}
