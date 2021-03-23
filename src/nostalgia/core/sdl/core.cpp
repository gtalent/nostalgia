/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <SDL.h>

#include <nostalgia/core/config.hpp>
#include <nostalgia/core/core.hpp>
#include <nostalgia/core/gfx.hpp>
#include <nostalgia/core/input.hpp>

#include "core.hpp"

namespace nostalgia::core {

static event_handler g_eventHandler = nullptr;
static uint64_t g_wakeupTime;

void draw(Context *ctx);

ox::Error init(Context *ctx) {
	oxReturnError(initGfx(ctx));
	return OxError(0);
}

ox::Error run(Context *ctx) {
	const auto id = ctx->windowerData<SdlImplData>();
	// try adaptive vsync
	if (SDL_GL_SetSwapInterval(config::SdlVsyncOption) < 0) {
		oxTrace("nostalgia::core::sdl", "Could not enable adaptive vsync, falling back on vsync");
		SDL_GL_SetSwapInterval(1); // fallback on normal vsync
	}
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
		const auto ticks = ticksMs();
		if (g_wakeupTime <= ticks && g_eventHandler) {
			auto sleepTime = g_eventHandler(ctx);
			if (sleepTime >= 0) {
				g_wakeupTime = ticks + static_cast<unsigned>(sleepTime);
			} else {
				g_wakeupTime = ~uint64_t(0);
			}
		}
		draw(ctx);
		SDL_GL_SwapWindow(id->window);
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
