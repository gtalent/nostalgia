/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

namespace nostalgia::core {

typedef struct { uint32_t data[8]; } __attribute__((aligned(4))) Tile, Tile4;
// d-tile: double-sized tile (8bpp)
typedef struct { uint32_t data[16]; }  __attribute__((aligned(4))) Tile8;
// tile block: 32x16 tiles, 16x16 d-tiles
using Palette = uint16_t[256];
using CharBlock = Tile[512];
using CharBlock8 = Tile8[256];

struct __attribute__((packed)) GbaImageDataHeader {
	uint8_t bpp = 0;
	uint16_t tileCount = 0;
};

struct __attribute__((packed)) GbaImageData {
	GbaImageDataHeader header;
	uint8_t colors = 0; // colors in the palette
	Palette __attribute__((packed)) pal = {};
	uint8_t tiles[1];
};

}
