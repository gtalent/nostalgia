/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace nostalgia {
namespace core {

typedef struct { uint32_t data[8]; } Tile, Tile4;
// d-tile: double-sized tile (8bpp)
typedef struct { uint32_t data[16]; } Tile8;
// tile block: 32x16 tiles, 16x16 d-tiles
typedef unsigned short Pallete[256];
typedef Tile  CharBlock[512];
typedef Tile8 CharBlock8[256];

struct GbaImageData {
	Pallete pal;
	uint8_t tiles[1];
};

}
}
