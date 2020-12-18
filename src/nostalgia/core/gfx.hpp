/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

#include "context.hpp"

namespace nostalgia::core {

extern char charMap[128];

enum class TileSheetSpace {
	Background,
	Sprite
};

using Color16 = uint16_t;

/**
 * Nostalgia Core logically uses 16 bit colors, but must translate that to 32
 * bit colors in some implementations.
 */
using Color32 = uint32_t;

struct NostalgiaPalette {
	static constexpr auto TypeName = "net.drinkingtea.nostalgia.core.NostalgiaPalette";
	static constexpr auto Fields = 1;
	static constexpr auto TypeVersion = 1;
	ox::Vector<Color16> colors;
};

struct NostalgiaGraphic {
	static constexpr auto TypeName = "net.drinkingtea.nostalgia.core.NostalgiaGraphic";
	static constexpr auto Fields = 6;
	static constexpr auto TypeVersion = 1;
	int8_t bpp = 0;
	// rows and columns are really only used by TileSheetEditor
	int rows = 1;
	int columns = 1;
	ox::FileAddress defaultPalette;
	NostalgiaPalette pal;
	ox::Vector<uint8_t> tiles;
};

template<typename T>
ox::Error model(T *io, NostalgiaPalette *pal) {
	io->template setTypeInfo<NostalgiaPalette>();
	oxReturnError(io->field("colors", &pal->colors));
	return OxError(0);
}

template<typename T>
ox::Error model(T *io, NostalgiaGraphic *ng) {
	io->template setTypeInfo<NostalgiaGraphic>();
	oxReturnError(io->field("bpp", &ng->bpp));
	oxReturnError(io->field("rows", &ng->rows));
	oxReturnError(io->field("columns", &ng->columns));
	oxReturnError(io->field("defaultPalette", &ng->defaultPalette));
	oxReturnError(io->field("pal", &ng->pal));
	oxReturnError(io->field("tiles", &ng->tiles));
	return OxError(0);
}

ox::Error initGfx(Context *ctx);

ox::Error shutdownGfx(Context*);

ox::Error initConsole(Context *ctx);

/**
 * @param section describes which section of the selected TileSheetSpace to use (e.g. MEM_PALLETE_BG[section])
 */
ox::Error loadBgTileSheet(Context *ctx, int section, ox::FileAddress tilesheet, ox::FileAddress palette = nullptr);

ox::Error loadSpriteTileSheet(Context *ctx,
                                            int section,
                                            ox::FileAddress tilesheetAddr,
                                            ox::FileAddress paletteAddr);

[[nodiscard]] Color32 toColor32(Color16 nc) noexcept;

[[nodiscard]] uint8_t red32(Color16 c) noexcept;

[[nodiscard]] uint8_t green32(Color16 c) noexcept;

[[nodiscard]] uint8_t blue32(Color16 c) noexcept;

[[nodiscard]] constexpr uint8_t alpha32(Color16 c) noexcept {
	return (c >> 15) * 255;
}


[[nodiscard]] constexpr Color16 color16(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return r | (g << 5) | (b << 10) | (a << 15);
}

[[nodiscard]] uint8_t red32(Color32 c) noexcept;

[[nodiscard]] uint8_t green32(Color32 c) noexcept;

[[nodiscard]] uint8_t blue32(Color32 c) noexcept;

[[nodiscard]] constexpr uint8_t red16(Color16 c) noexcept {
	return c & 0b0000000000011111;
}

[[nodiscard]] constexpr uint8_t green16(Color16 c) noexcept {
	return (c & 0b0000001111100000) >> 5;
}

[[nodiscard]] constexpr uint8_t blue16(Color16 c) noexcept {
	return (c & 0b0111110000000000) >> 10;
}

[[nodiscard]] constexpr uint8_t alpha16(Color16 c) noexcept {
	return c >> 15;
}

void puts(Context *ctx, int column, int row, const char *str);

void setTile(Context *ctx, int layer, int column, int row, uint8_t tile);

void clearTileLayer(Context*, int layer);

void hideSprite(Context*, unsigned);

void setSprite(Context*, unsigned idx, unsigned x, unsigned y, unsigned tileIdx, unsigned spriteShape = 0, unsigned spriteSize = 0, unsigned flipX = 0);

}
