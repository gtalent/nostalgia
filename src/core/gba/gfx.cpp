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

	auto fs = (FileStore32*) findMedia();
	REG_BG0CNT = (28 << 8) | 1;
	REG_BG0CNT |= (1 << 7); // set to use 8 bits per pixel
	if (fs) {
		FileStore32::FsSize_t readSize = 0;
		fs->read(1, 512, 8 * 64 * 36, &TILE8_ADDR[0][1], nullptr);
		fs->read(1, 0, 512, &MEM_PALLETE_BG[0], &readSize);
	}

	puts(296, "HELLO, WORLD!");

	return 0;
}

void puts(int loc, const char *str) {
	for (int i = 0; str[i]; i++) {
		MEM_BG_MAP[28][loc + i] = charMap[(int) str[i]];
	}
}

}
}
