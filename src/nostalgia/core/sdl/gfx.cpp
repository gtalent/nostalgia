/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>
#include <vector>

#include <SDL.h>

#include <ox/mc/read.hpp>

#include <nostalgia/core/gfx.hpp>

namespace nostalgia::core {

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

static std::array<SDL_Texture*, 4> bgTextures;
static std::vector<std::vector<int>> bgTileMaps(128, std::vector<int>(128, 0));

[[nodiscard]] static ox::ValErr<ox::Vector<uint8_t>> readFile(Context *ctx, const ox::FileAddress &file) {
	auto [stat, err] = ctx->rom->stat(file);
	oxReturnError(err);
	ox::Vector<uint8_t> buff(stat.size);
	oxReturnError(ctx->rom->read(file, buff.data(), buff.size()));
	return buff;
}

template<typename T>
[[nodiscard]] ox::ValErr<T> readMC(Context *ctx, const ox::FileAddress &file) {
	auto [buff, err] = readFile(ctx, file);
	oxReturnError(err);
	T t;
	oxReturnError(ox::readMC(buff.data(), buff.size(), &t));
	return t;
}

ox::Error initGfx(Context*) {
	window = SDL_CreateWindow("nostalgia", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	return OxError(window == nullptr);
}

ox::Error shutdownGfx() {
	for (auto tex : bgTextures) {
		SDL_DestroyTexture(tex);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return OxError(0);
}

ox::Error initConsole(Context*) {
	return OxError(1);
}

SDL_Color createSDL_Color(Color nc) {
	SDL_Color c;
	// extract the color chanels and scale them up for a 24 bit color
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

ox::Error loadTileSheet(Context *ctx,
                        TileSheetSpace tss,
                        int section,
                        ox::FileAddress tilesheetPath,
                        ox::FileAddress palettePath) {
	auto [tilesheet, tserr] = readMC<NostalgiaGraphic>(ctx, tilesheetPath);
	oxReturnError(tserr);
	NostalgiaPalette palette;
	if (palettePath) {
		oxReturnError(readMC<NostalgiaPalette>(ctx, palettePath).get(&palette));
	} else {
		palette = tilesheet.pal;
	}

	const auto bytesPerTile = tilesheet.bpp == 8 ? 64 : 32;
	const auto tiles = tilesheet.tiles.size() / bytesPerTile;
	const int width = 8;
	const int height = 8 * tiles;
	const auto format = SDL_PIXELFORMAT_INDEX8;
	auto surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 16, format);
	auto sdlPalette = createSDL_Palette(palette);
	SDL_SetSurfacePalette(surface, sdlPalette);
	if (bytesPerTile == 1) {
		SDL_memcpy(surface->pixels, tilesheet.tiles.data(), bytesPerTile * tiles);
	} else {
		for (std::size_t i = 0; i < tilesheet.tiles.size(); ++i) {
			static_cast<uint8_t*>(surface->pixels)[i * 2 + 0] = tilesheet.tiles[i] & 0xF; 
			static_cast<uint8_t*>(surface->pixels)[i * 2 + 1] = tilesheet.tiles[i] >> 4; 
		}
	}

	auto texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	SDL_FreePalette(sdlPalette);

	if (tss == TileSheetSpace::Background) {
		if (bgTextures[section]) {
			SDL_DestroyTexture(bgTextures[section]);
		}
		bgTextures[section] = texture;
	}

	return OxError(0);
}

void drawBackground(std::size_t bg) {
	//oxTrace("nostalgia::core::drawBackground") << "Drawing background";
	SDL_Rect src = {}, dst = {};
	src.x = 0;
	src.w = 8;
	src.h = 8;
	dst.x = 0;
	dst.y = 0;
	dst.w = 64;
	dst.h = 64;
	const auto tex = bgTextures[bg];
	if (tex) {
		for (auto &m : bgTileMaps) {
			for (auto t : m) {
				src.y = t * 8;
				SDL_RenderCopy(renderer, tex, &src, &dst);
				dst.y += 64;
			}
			dst.x += 64;
			dst.y = 0;
		}
	}
}

void draw() {
	SDL_RenderClear(renderer);
	drawBackground(0);
	drawBackground(1);
	drawBackground(2);
	drawBackground(3);
	SDL_RenderPresent(renderer);
}

void puts(Context*, int, const char*) {
}

void setTile(Context*, int, int, int, uint8_t) {
}

}
