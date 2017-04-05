/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/types.hpp>
#include "registers.hpp"
#include "dirt.h"

namespace nostalgia {
namespace core {

using namespace ox::std;

typedef struct { uint32_t data[8]; } Tile, Tile4;
// d-tile: double-sized tile (8bpp)
typedef struct { uint32_t data[16]; } Tile8;
// tile block: 32x16 tiles, 16x16 d-tiles
typedef struct { unsigned short data[256]; } Pallete;
typedef Tile  CharBlock[512];
typedef Tile8 CharBlock8[256];

#define TILE_ADDR  ((CharBlock*) 0x06000000)
#define TILE8_ADDR ((CharBlock8*) 0x06000000)

#define PALLETE_BG     ((unsigned short*) 0x05000000)
#define PALLETE_SPRITE ((unsigned short*) 0x05000200)

typedef uint16_t BgMapTile[1024];
#define BG_MAP ((BgMapTile*) 0x06000000)

ox::std::Error initGfx() {
	/* Sprite Mode ----\ */
	/*             ---\| */
	/* Background 0 -\|| */
	/* Objects -----\||| */
	/*              |||| */
	REG_DISPCNT = 0x1100;

	TILE_ADDR[0][1] = *(Tile*) dirtTiles;

	for (auto i = 0; i < (dirtPalLen / 2); i++) {
		PALLETE_BG[i] = dirtPal[i];
	}

	BG_MAP[28][52] = 1;
	REG_BG0CNT = (28 << 8) | 1;

	return 0;
}

}
}
