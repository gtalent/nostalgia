/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <SDL.h>

#include <nostalgia/core/core.hpp>

namespace nostalgia::core {

void draw(Context *ctx);

ox::Error run(Context *ctx) {
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
		SDL_Delay(1);
	}
	return OxError(0);
}

}
