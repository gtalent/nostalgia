/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <SDL.h>

#include <nostalgia/core/gfx.hpp>
#include <nostalgia/core/input.hpp>
#include <nostalgia/core/core.hpp>

#include "core.hpp"

namespace nostalgia::core {

static event_handler g_eventHandler = nullptr;

void draw(Context *ctx);

ox::Error init(Context *ctx) {
	oxReturnError(initGfx(ctx));
	return OxError(0);
}

ox::Error run(Context *ctx) {
	auto id = ctx->windowerData<SdlImplData>();
	for (auto running = true; running;) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_q) {
						running = false;
					}
					break;
				case SDL_QUIT: {
					running = false;
					break;
				}
			}
		}
		draw(ctx);
		SDL_GL_SwapWindow(id->window);
		SDL_Delay(1);
	}
	return OxError(0);
}

void setEventHandler(event_handler h) {
	g_eventHandler = h;
}

uint64_t ticksMs() {
	return SDL_GetTicks();
}

bool buttonDown(Key) {
	return false;
}

}
