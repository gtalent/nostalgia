/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "gfx.hpp"

namespace nostalgia::core {

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

Color32 toColor32(Color16 nc) noexcept {
	Color32 r = ((nc & 0b0000000000011111) >> 0) * 8;
	Color32 g = ((nc & 0b0000001111100000) >> 5) * 8;
	Color32 b = ((nc & 0b0111110000000000) >> 10) * 8;
	Color32 a = 255;
	return a | (b << 8) | (g << 16) | (r << 24);
}


uint8_t red32(Color32 c) noexcept {
	return (c & 0x000000ff) >> 0;
}

uint8_t green32(Color32 c) noexcept {
	return (c & 0x0000ff00) >> 8;
}

uint8_t blue32(Color32 c) noexcept {
	return (c & 0x00ff0000) >> 16;
}


uint8_t red32(Color16 c) noexcept {
	return red16(c) * 8;
}

uint8_t green32(Color16 c) noexcept {
	return green16(c) * 8;
}

uint8_t blue32(Color16 c) noexcept {
	return blue16(c) * 8;
}

void puts(Context *ctx, int column, int row, const char *str) {
	for (int i = 0; str[i]; i++) {
		setTile(ctx, 0, column + i, row, charMap[static_cast<int>(str[i])]);
	}
}

static_assert(color16(0, 31, 0, 0) == 992);
static_assert(color16(16, 31, 0, 0) == 1008);
static_assert(color16(16, 31, 8, 0) == 9200);
static_assert(color16(16, 31, 8, 1) == 41968);

}
