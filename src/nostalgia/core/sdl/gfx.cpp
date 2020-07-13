/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>
#include <vector>
#ifdef NOST_FPS_PRINT
#include <iostream>
#endif

#include <SDL.h>

#include <ox/claw/read.hpp>

#include <nostalgia/core/gfx.hpp>

namespace nostalgia::core {

using TileMap = std::array<std::array<int, 128>, 128>;

struct SdlImplData {
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	std::array<SDL_Texture*, 4> bgTextures{};
	std::array<TileMap, 4> bgTileMaps{};
	int64_t prevFpsCheckTime = 0;
	uint64_t draws = 0;
};

constexpr auto Scale = 5;

[[nodiscard]] static ox::ValErr<ox::Vector<char>> readFile(Context *ctx, const ox::FileAddress &file) {
	auto [stat, err] = ctx->rom->stat(file);
	oxReturnError(err);
	ox::Vector<char> buff(stat.size);
	oxReturnError(ctx->rom->read(file, buff.data(), buff.size()));
	return buff;
}

template<typename T>
[[nodiscard]] ox::ValErr<T> readObj(Context *ctx, const ox::FileAddress &file) {
	auto [buff, err] = readFile(ctx, file);
	oxReturnError(err);
	T t;
	oxReturnError(ox::readClaw(buff.data(), buff.size(), &t));
	return t;
}

ox::Error initGfx(Context *ctx) {
	auto id = new SdlImplData;
	ctx->setImplData(id);
	id->window = SDL_CreateWindow("nostalgia", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 240 * Scale, 160 * Scale,
	                              SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	id->renderer = SDL_CreateRenderer(id->window, -1, SDL_RENDERER_ACCELERATED);
	return OxError(id->window == nullptr);
}

ox::Error shutdownGfx(Context *ctx) {
	auto id = ctx->implData<SdlImplData>();
	for (auto tex : id->bgTextures) {
		SDL_DestroyTexture(tex);
	}
	SDL_DestroyRenderer(id->renderer);
	SDL_DestroyWindow(id->window);
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
	auto id = ctx->implData<SdlImplData>();
	auto [tilesheet, tserr] = readObj<NostalgiaGraphic>(ctx, tilesheetPath);
	oxReturnError(tserr);
	NostalgiaPalette palette;
	if (!palettePath) {
		palettePath = tilesheet.defaultPalette;
	}
	oxReturnError(readObj<NostalgiaPalette>(ctx, palettePath).get(&palette));

	const unsigned bytesPerTile = tilesheet.bpp == 8 ? 64 : 32;
	const auto tiles = tilesheet.tiles.size() / bytesPerTile;
	const int width = 8;
	const int height = 8 * tiles;
	const auto format = SDL_PIXELFORMAT_INDEX8;
	auto surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 16, format);
	auto sdlPalette = createSDL_Palette(palette);
	SDL_SetSurfacePalette(surface, sdlPalette);
	if (bytesPerTile == 64) {
		SDL_memcpy(surface->pixels, tilesheet.tiles.data(), bytesPerTile * tiles);
	} else {
		for (std::size_t i = 0; i < tilesheet.tiles.size(); ++i) {
			static_cast<uint8_t*>(surface->pixels)[i * 2 + 0] = tilesheet.tiles[i] & 0xF;
			static_cast<uint8_t*>(surface->pixels)[i * 2 + 1] = tilesheet.tiles[i] >> 4;
		}
	}

	auto texture = SDL_CreateTextureFromSurface(id->renderer, surface);
	SDL_FreeSurface(surface);
	SDL_FreePalette(sdlPalette);

	auto sectionIdx = static_cast<unsigned>(section);
	if (id->bgTextures[sectionIdx]) {
		SDL_DestroyTexture(id->bgTextures[sectionIdx]);
	}
	id->bgTextures[sectionIdx] = texture;

	return OxError(0);
}

ox::Error loadSpriteTileSheet(Context*,
                              int,
                              ox::FileAddress,
                              ox::FileAddress) {
	return OxError(0);
}

void drawBackground(Context *ctx, const TileMap &tm, SDL_Texture *tex) {
	if (tex) {
		constexpr auto DstSize = 8 * Scale;
		auto id = ctx->implData<SdlImplData>();
		//oxTrace("nostalgia::core::drawBackground") << "Drawing background";
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
				SDL_RenderCopy(id->renderer, tex, &src, &dst);
				dst.x += DstSize;
			}
			dst.x = 0;
			dst.y += DstSize;
		}
	}
}

void draw(Context *ctx) {
	auto id = ctx->implData<SdlImplData>();
	++id->draws;
	if (id->draws >= 5000) {
		using namespace std::chrono;
		const auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		const auto duration = static_cast<double>(now - id->prevFpsCheckTime) / 1000.0;
		const auto fps = static_cast<int>(static_cast<double>(id->draws) / duration);
#ifdef NOST_FPS_PRINT
		std::cout << "FPS: " << fps << '\n';
#endif
		oxTrace("nostalgia::core::gfx::fps") << "FPS:" << fps;
		id->prevFpsCheckTime = now;
		id->draws = 0;
	}
	SDL_RenderClear(id->renderer);
	for (std::size_t i = 0; i < id->bgTileMaps.size(); i++) {
		auto tex = id->bgTextures[i];
		auto &tm = id->bgTileMaps[i];
		drawBackground(ctx, tm, tex);
	}
	SDL_RenderPresent(id->renderer);
}

void setTile(Context *ctx, int layer, int column, int row, uint8_t tile) {
	auto id = ctx->implData<SdlImplData>();
	auto z = static_cast<unsigned>(layer);
	auto y = static_cast<unsigned>(row);
	auto x = static_cast<unsigned>(column);
	id->bgTileMaps[z][y][x] = tile;
}

void setSprite(unsigned, unsigned, unsigned, unsigned) {
}

}
