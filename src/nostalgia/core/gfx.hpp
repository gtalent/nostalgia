/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

#include "context.hpp"
#include "types.hpp"

namespace nostalgia::core {

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
	static constexpr auto Fields = 1;
	ox::Vector<Color16> colors;
};

struct NostalgiaGraphic {
	static constexpr auto Fields = 4;
	uint8_t bpp = 0;
	ox::FileAddress defaultPalette;
	NostalgiaPalette pal;
	ox::Vector<uint8_t> tiles;
};

template<typename T>
ox::Error model(T *io, NostalgiaPalette *pal) {
	io->setTypeInfo("nostalgia::core::NostalgiaPalette", NostalgiaPalette::Fields);
	oxReturnError(io->field("colors", &pal->colors));
	return OxError(0);
}

template<typename T>
ox::Error model(T *io, NostalgiaGraphic *ng) {
	io->setTypeInfo("nostalgia::core::NostalgiaGraphic", NostalgiaGraphic::Fields);
	oxReturnError(io->field("bpp", &ng->bpp));
	oxReturnError(io->field("defaultPalette", &ng->defaultPalette));
	oxReturnError(io->field("pal", &ng->pal));
	oxReturnError(io->field("tiles", &ng->tiles));
	return OxError(0);
}

[[nodiscard]] ox::Error initGfx(Context *ctx);

[[nodiscard]] ox::Error shutdownGfx();

[[nodiscard]] ox::Error initConsole(Context *ctx);

/**
 * @param section describes which section of the selected TileSheetSpace to use (e.g. MEM_PALLETE_BG[section])
 */
[[nodiscard]] ox::Error loadTileSheet(Context *ctx, TileSheetSpace tss, int section, ox::FileAddress tilesheet, ox::FileAddress palette = nullptr);

[[nodiscard]] Color32 toColor32(Color16 nc);

void puts(Context *ctx, int column, int row, const char *str);

void setTile(Context *ctx, int layer, int column, int row, uint8_t tile);

}
