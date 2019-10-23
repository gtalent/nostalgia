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

using Color = uint16_t;

struct NostalgiaPalette {
	static constexpr auto Fields = 1;
	ox::Vector<Color> colors;
};

struct NostalgiaGraphic {
	static constexpr auto Fields = 4;
	uint8_t bpp = 0;
	ox::FileAddress defaultPalette;
	ox::Vector<Color> pal;
	ox::Vector<uint8_t> tiles;
};

template<typename T>
ox::Error modelWrite(T *io, NostalgiaGraphic *ng) {
	io->setTypeInfo("nostalgia::core::NostalgiaGraphic", NostalgiaGraphic::Fields);
	oxReturnError(io->field("bpp", &ng->bpp));
	oxReturnError(io->field("defaultPalette", &ng->defaultPalette));
	oxReturnError(io->field("pal", &ng->pal));
	oxReturnError(io->field("tiles", &ng->tiles));
	return OxError(0);
}

template<typename T>
ox::Error modelWrite(T *io, NostalgiaPalette *pal) {
	io->setTypeInfo("nostalgia::core::NostalgiaPalette", NostalgiaPalette::Fields);
	oxReturnError(io->field("colors", &pal->colors));
	return OxError(0);
}

ox::Error initGfx(Context *ctx);

ox::Error initConsole(Context *ctx);

ox::Error loadTileSheet(Context *ctx, ox::FileAddress file);

void puts(Context *ctx, int loc, const char *str);

void setTile(Context *ctx, int layer, int column, int row, uint8_t tile);

}
