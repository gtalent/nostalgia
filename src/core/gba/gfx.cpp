/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/filesystem.hpp>
#include <ox/std/std.hpp>
#include "addresses.hpp"
#include "media.hpp"
#include "gba.hpp"
#include "../gfx.hpp"

namespace nostalgia {
namespace core {

using namespace ox::fs;

#define TILE_ADDR  ((CharBlock*) 0x06000000)
#define TILE8_ADDR ((CharBlock8*) 0x06000000)

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
	0,
	38, // !
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
	37, // ,
	0,
	0,
	0,
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
	0,
	0,
	0,
	0,
	0,
	0,
	0,
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
};

ox::Error initGfx() {
	/* Sprite Mode ----\ */
	/*             ---\| */
	/* Background 0 -\|| */
	/* Objects -----\||| */
	/*              |||| */
	REG_DISPCNT = 0x1100;

	return 0;
}

void initConsole() {
	auto fs = (FileStore32*) findMedia();

	GbaImageData imgData;
	fs->read(101, __builtin_offsetof(GbaImageData, tileCount),
	         sizeof(imgData.tileCount) + sizeof(imgData.bpp),
	         &imgData.tileCount, nullptr);

	REG_BG0CNT = (28 << 8) | 1;
	if (imgData.bpp == 8) {
		REG_BG0CNT |= (1 << 7); // set to use 8 bits per pixel
	}
	if (fs) {
		fs->read(101, __builtin_offsetof(GbaImageData, tiles), 64 * 38, &TILE8_ADDR[0][1], nullptr);
		fs->read(101, __builtin_offsetof(GbaImageData, pal), 512, &MEM_PALLETE_BG[0], nullptr);
	}
}

void puts(int loc, const char *str) {
	for (int i = 0; str[i]; i++) {
		MEM_BG_MAP[28][loc + i] = charMap[(int) str[i]];
	}
}

}
}
