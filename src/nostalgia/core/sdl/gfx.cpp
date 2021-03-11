/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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

using TileMap = std::array<std::array<int, 128>, 128>;

constexpr auto Scale = 5;

static ox::Result<ox::Vector<char>> readFile(Context *ctx, const ox::FileAddress &file) {
	auto [stat, err] = ctx->rom->stat(file);
	oxReturnError(err);
	ox::Vector<char> buff(stat.size);
	oxReturnError(ctx->rom->read(file, buff.data(), buff.size()));
	return buff;
}

template<typename T>
ox::Result<T> readObj(Context *ctx, const ox::FileAddress &file) {
	auto [buff, err] = readFile(ctx, file);
	oxReturnError(err);
	T t;
	oxReturnError(ox::readClaw(buff.data(), buff.size(), &t));
	return t;
}

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

ox::Error initConsole(Context *ctx) {
	constexpr auto TilesheetAddr = "/TileSheets/Charset.ng";
	return loadBgTileSheet(ctx, 0, TilesheetAddr);
}

SDL_Color createSDL_Color(Color16 nc) {
	SDL_Color c;
	// extract the color channels and scale them up for a 24 bit color
	c.r = ((nc & 0b0000000000011111) >> 0) * 8;
	c.g = ((nc & 0b0000001111100000) >> 5) * 8;
	c.b = ((nc & 0b0111110000000000) >> 10) * 8;
	c.a = 1;
	return c;
}

SDL_Palette *createSDL_Palette(const NostalgiaPalette &npal) {
	auto pal = SDL_AllocPalette(npal.colors.size());
	for (std::size_t i = 0; i < npal.colors.size(); ++i) {
		pal->colors[i] = createSDL_Color(npal.colors[i]);
	}
	return pal;
}

ox::Error loadBgTileSheet(Context *ctx,
                          int section,
                          ox::FileAddress tilesheetPath,
                          ox::FileAddress palettePath) {
	//auto id = ctx->windowerData<SdlImplData>();
	const auto [tilesheet, tserr] = readObj<NostalgiaGraphic>(ctx, tilesheetPath);
	oxReturnError(tserr);
	NostalgiaPalette palette;
	if (!palettePath) {
		palettePath = tilesheet.defaultPalette;
	}
	oxReturnError(readObj<NostalgiaPalette>(ctx, palettePath).get(&palette));

	const unsigned bytesPerTile = tilesheet.bpp == 8 ? 64 : 32;
	const auto tiles = tilesheet.pixels.size() / bytesPerTile;
	const int width = 8;
	const int height = 8 * tiles;
	//const auto format = SDL_PIXELFORMAT_INDEX8;
	//const auto sdlPalette = createSDL_Palette(palette);
	std::vector<uint32_t> pixels;
	if (bytesPerTile == 64) { // 8 BPP
		pixels.resize(tilesheet.pixels.size());
		for (std::size_t i = 0; i < tilesheet.pixels.size(); ++i) {
			pixels[i] = toColor32(palette.colors[tilesheet.pixels[i]]);
		}
	} else { // 4 BPP
		pixels.resize(tilesheet.pixels.size() * 2);
		for (std::size_t i = 0; i < tilesheet.pixels.size(); ++i) {
			pixels[i * 2 + 0] = toColor32(palette.colors[tilesheet.pixels[i] & 0xF]);
			pixels[i * 2 + 1] = toColor32(palette.colors[tilesheet.pixels[i] >> 4]);
		}
	}

	oxReturnError(renderer::loadTexture(ctx, section, pixels.data(), width, height));

	//auto texture = SDL_CreateTextureFromSurface(nullptr, surface);
	//SDL_FreeSurface(surface);
	//SDL_FreePalette(sdlPalette);

	//auto sectionIdx = static_cast<unsigned>(section);
	//if (id->bgTextures[sectionIdx]) {
	//	SDL_DestroyTexture(id->bgTextures[sectionIdx]);
	//}
	//id->bgTextures[sectionIdx] = texture;

	return OxError(0);
}

ox::Error loadSpriteTileSheet(Context*,
                              int,
                              ox::FileAddress,
                              ox::FileAddress) {
	return OxError(0);
}

void drawBackground(Context*, const TileMap &tm, SDL_Texture *tex) {
	if (tex) {
		constexpr auto DstSize = 8 * Scale;
		oxTracef("nostalgia::core::sdl::drawBackground", "Drawing background");
		SDL_Rect src = {}, dst = {};
		src.x = 0;
		src.w = 8;
		src.h = 8;
		dst.x = 0;
		dst.y = 0;
		dst.w = DstSize;
		dst.h = DstSize;
		for (auto &m : tm) {
			for (auto t : m) {
				src.y = t * 8;
				SDL_RenderCopy(nullptr, tex, &src, &dst);
				dst.x += DstSize;
			}
			dst.x = 0;
			dst.y += DstSize;
		}
	}
}

}
