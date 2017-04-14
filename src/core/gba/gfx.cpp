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

namespace nostalgia {
namespace core {

using namespace ox::fs;

#define TILE_ADDR  ((CharBlock*) 0x06000000)
#define TILE8_ADDR ((CharBlock8*) 0x06000000)

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
		fs->read(1, 511, 8 * 64 * 36, &TILE8_ADDR[0][1], nullptr);
		fs->read(1, 0, 512, &MEM_PALLETE_BG[0], &readSize);
	}

	MEM_BG_MAP[28][106] = 1;
	MEM_BG_MAP[28][107] = 1;

	return 0;
}

}
}
