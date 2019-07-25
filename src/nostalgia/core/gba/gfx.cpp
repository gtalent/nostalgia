/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/fs.hpp>
#include <ox/std/std.hpp>

#include "../media.hpp"

#include "addresses.hpp"
#include "gba.hpp"
#include "panic.hpp"

#include "../gfx.hpp"

namespace nostalgia::core {

using namespace ox;

#define TILE_ADDR  ((CharBlock*) 0x06000000)
#define TILE8_ADDR ((CharBlock8*) 0x06000000)

const auto GBA_TILE_COLUMNS = 32;
const auto GBA_TILE_ROWS = 32;

// map ASCII values to the nostalgia charset
static char charMap[128] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,  // space
	38, // !
	0,  // "
	0,  // #
	0,  // $
	0,  // %
	0,  // &
	0,  // '
	42, // (
	43, // )
	0,  // *
	0,  // +
	37, // ,
	0,  // -
	39, // .
	0,  // /
	27, // 0
	28, // 1
	29, // 2
	30, // 3
	31, // 4
	32, // 5
	33, // 6
	34, // 7
	35, // 8
	36, // 9
	40, // :
	0,  // ;
	0,  // <
	41, // =
	0,  // >
	0,  // ?
	0,  // @
	1,  // A
	2,  // B
	3,  // C
	4,  // D
	5,  // E
	6,  // F
	7,  // G
	8,  // H
	9,  // I
	10, // J
	11, // K
	12, // L
	13, // M
	14, // N
	15, // O
	16, // P
	17, // Q
	18, // R
	19, // S
	20, // T
	21, // U
	22, // V
	23, // W
	24, // X
	25, // Y
	26, // Z
	44, // [
	0,  // backslash
	45, // ]
	0,  // ^
	0,  // _
	0,  // `
	1,  // a
	2,  // b
	3,  // c
	4,  // d
	5,  // e
	6,  // f
	7,  // g
	8,  // h
	9,  // i
	10, // j
	11, // k
	12, // l
	13, // m
	14, // n
	15, // o
	16, // p
	17, // q
	18, // r
	19, // s
	20, // t
	21, // u
	22, // v
	23, // w
	24, // x
	25, // y
	26, // z
	46, // {
	0,  // |
	48, // }
	0,  // ~
};

ox::Error initGfx(Context*) {
	/* Sprite Mode ----\ */
	/*             ---\| */
	/* Background 0 -\|| */
	/* Objects -----\||| */
	/*              |||| */
	REG_DISPCNT = 0x1100;

	return 0;
}

// Do NOT use Context in the GBA version of this function.
ox::Error initConsole(Context*) {
	const auto PaletteStart = sizeof(GbaImageDataHeader);
	ox::Error err = 0;
	ox::FileStore32 fs(loadRom(), 32 * ox::units::MB);
	ox::FileSystem32 fileSystem(fs);
	const auto CharsetInode = fileSystem.stat("/TileSheets/Charset.ng").value.inode;

	GbaImageDataHeader imgData;

	REG_BG0CNT = (28 << 8) | 1;
	if (fs.valid()) {
		// load the header
		err |= fs.read(CharsetInode, 0, sizeof(imgData), &imgData, nullptr);

		// load palette
		err |= fs.read(CharsetInode, PaletteStart,
		               512, (uint16_t*) &MEM_PALLETE_BG[0], nullptr);

		if (imgData.bpp == 4) {
			err |= fs.read(CharsetInode, __builtin_offsetof(GbaImageData, tiles),
			               sizeof(Tile) * imgData.tileCount, (uint16_t*) &TILE_ADDR[0][1], nullptr);
		} else if (imgData.bpp == 8) {
			REG_BG0CNT |= (1 << 7); // set to use 8 bits per pixel
			err |= fs.read(CharsetInode, __builtin_offsetof(GbaImageData, tiles),
			               sizeof(Tile8) * imgData.tileCount, (uint16_t*) &TILE8_ADDR[0][1], nullptr);
		} else {
			err = 1;
		}
	} else {
		err = 1;
	}
	return err;
}

ox::Error loadTileSheet(Context*, InodeId_t inode) {
	ox::Error err = 0;
	const auto PaletteStart = sizeof(GbaImageDataHeader);
	GbaImageDataHeader imgData;

	ox::FileStore32 fs(loadRom(), 32 * 1024 * 1024);
	REG_BG0CNT = (28 << 8) | 1;
	if (fs.valid()) {
		// load the header
		err |= fs.read(inode, 0, sizeof(imgData), &imgData, nullptr);

		// load palette
		err |= fs.read(inode, PaletteStart,
		                512, (uint16_t*) &MEM_PALLETE_BG[0], nullptr);

		if (imgData.bpp == 4) {
			err |= fs.read(inode, __builtin_offsetof(GbaImageData, tiles),
			                sizeof(Tile) * imgData.tileCount, (uint16_t*) &TILE_ADDR[0][1], nullptr);
		} else if (imgData.bpp == 8) {
			REG_BG0CNT |= (1 << 7); // set to use 8 bits per pixel
			err |= fs.read(inode, __builtin_offsetof(GbaImageData, tiles),
			                sizeof(Tile8) * imgData.tileCount, (uint16_t*) &TILE8_ADDR[0][1], nullptr);
		} else {
			err = 1;
		}
	} else {
		err = 1;
	}
	return err;
}

// Do NOT use Context in the GBA version of this function.
void puts(Context*, int loc, const char *str) {
	for (int i = 0; str[i]; i++) {
		MEM_BG_MAP[28][loc + i] = charMap[(int) str[i]];
	}
}

void setTile(Context*, int layer, int column, int row, uint8_t tile) {
	if (column < GBA_TILE_COLUMNS && row < GBA_TILE_ROWS) {
		MEM_BG_MAP[28 + layer][row * GBA_TILE_COLUMNS + column] = tile;
	}
}

}
