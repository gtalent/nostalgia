/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>
#include <nostalgia/common/size.hpp>

#include "color.hpp"
#include "context.hpp"

namespace nostalgia::core {

extern char charMap[128];

enum class TileSheetSpace {
	Background,
	Sprite
};

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
	ox::Vector<uint8_t> pixels;
};

template<typename T>
constexpr ox::Error model(T *io, NostalgiaPalette *pal) {
	io->template setTypeInfo<NostalgiaPalette>();
	oxReturnError(io->field("colors", &pal->colors));
	return OxError(0);
}

template<typename T>
constexpr ox::Error model(T *io, NostalgiaGraphic *ng) {
	io->template setTypeInfo<NostalgiaGraphic>();
	oxReturnError(io->field("bpp", &ng->bpp));
	oxReturnError(io->field("rows", &ng->rows));
	oxReturnError(io->field("columns", &ng->columns));
	oxReturnError(io->field("defaultPalette", &ng->defaultPalette));
	oxReturnError(io->field("pal", &ng->pal));
	oxReturnError(io->field("pixels", &ng->pixels));
	return OxError(0);
}

struct Sprite {
	unsigned idx = 0;
	unsigned x = 0;
	unsigned y = 0;
	unsigned tileIdx = 0;
	unsigned spriteShape = 0;
	unsigned spriteSize = 0;
	unsigned flipX = 0;
};

ox::Error initGfx(Context *ctx);

ox::Error shutdownGfx(Context *ctx);

int getScreenWidth(Context *ctx);

int getScreenHeight(Context *ctx);

common::Size getScreenSize(Context *ctx);

[[nodiscard]]
uint8_t bgStatus(Context *ctx);

void setBgStatus(Context *ctx, uint32_t status);

bool bgStatus(Context *ctx, unsigned bg);

void setBgStatus(Context *ctx, unsigned bg, bool status);

ox::Error initConsole(Context *ctx);

/**
 * @param section describes which section of the selected TileSheetSpace to use (e.g. MEM_PALLETE_BG[section])
 */
ox::Error loadBgTileSheet(Context *ctx, int section, ox::FileAddress tilesheet, ox::FileAddress palette = nullptr);

ox::Error loadSpriteTileSheet(Context *ctx,
                              int section,
                              ox::FileAddress tilesheetAddr,
                              ox::FileAddress paletteAddr);

void puts(Context *ctx, int column, int row, const char *str);

void setTile(Context *ctx, int layer, int column, int row, uint8_t tile);

void clearTileLayer(Context *ctx, int layer);

void hideSprite(Context *ctx, unsigned);

void setSprite(Context *ctx, unsigned idx, unsigned x, unsigned y, unsigned tileIdx, unsigned spriteShape = 0, unsigned spriteSize = 0, unsigned flipX = 0);

void setSprite(Context *ctx, const Sprite &s);

}
