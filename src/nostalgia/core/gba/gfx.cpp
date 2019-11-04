/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/fs.hpp>
#include <ox/mc/mc.hpp>
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

constexpr auto GBA_TILE_COLUMNS = 32;
constexpr auto GBA_TILE_ROWS = 32;

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

struct GbaPaletteTarget {
	volatile uint16_t *palette = nullptr;
};

struct GbaTileMapTarget {
	static constexpr auto Fields = 4;
	volatile uint32_t *bgCtl = nullptr;
	ox::FileAddress defaultPalette;
	GbaPaletteTarget pal;
	volatile uint16_t *tileMap = nullptr;
};

template<typename T>
ox::Error modelRead(T *io, GbaPaletteTarget *t) {
	io->setTypeInfo("nostalgia::core::NostalgiaPalette", NostalgiaPalette::Fields);
	oxReturnError(io->template field<Color>("colors", [t](auto i, Color *c) {
		t->palette[i] = *c;
		return OxError(0);
	}));
	return OxError(0);
}

template<typename T>
ox::Error modelRead(T *io, GbaTileMapTarget *t) {
	io->setTypeInfo("nostalgia::core::NostalgiaGraphic", NostalgiaGraphic::Fields);

	uint8_t bpp;
	oxReturnError(io->field("bpp", &bpp));
	constexpr auto Bpp8 = 1 << 7;
	*t->bgCtl = (*t->bgCtl | Bpp8) ^ Bpp8; // set to use 4 bits per pixel
	*t->bgCtl |= Bpp8; // set to use 8 bits per pixel
	*t->bgCtl = (28 << 8) | 1;

	oxReturnError(io->field("defaultPalette", &t->defaultPalette));
	oxReturnError(io->field("pal", &t->pal));
	uint16_t intermediate = 0;
	auto err = io->template field<uint8_t>("tileMap", [t, &intermediate](auto i, uint8_t *tile) {
		if (i & 1) { // i is odd
			intermediate |= static_cast<uint16_t>(*tile) << 8;
			t->tileMap[i / 2] = intermediate;
		} else { // i is even
			intermediate = *tile & 0x00ff;
		}
		return OxError(0);
	});
	return OxError(err);
}

ox::Error initGfx(Context*) {
	/* Sprite Mode ----\ */
	/*             ---\| */
	/* Background 0 -\|| */
	/* Objects -----\||| */
	/*              |||| */
	REG_DISPCNT = 0x1100;

	return OxError(0);
}

ox::Error shutdownGfx() {
	return OxError(0);
}

[[nodiscard]] constexpr volatile uint32_t &bgCtl(int bg) noexcept {
	switch (bg) {
		case 0:
			return REG_BG0CNT;
		case 1:
			return REG_BG1CNT;
		case 2:
			return REG_BG2CNT;
		case 3:
			return REG_BG3CNT;
		default:
			panic("Looking up non-existent register");
			return REG_BG0CNT;
	}
}

// Do NOT rely on Context in the GBA version of this function.
ox::Error initConsole(Context *ctx) {
	constexpr auto TilesheetAddr = "/TileSheets/Charset.ng";
	constexpr auto PaletteAddr = "/Palettes/Charset.npal";
	if (!ctx) {
		ctx = new (ox_alloca(sizeof(Context))) Context();
		ox::FileStore32 fs(loadRom(), 32 * ox::units::MB);
		ctx->rom = new (ox_alloca(sizeof(ox::FileSystem32))) ox::FileSystem32(fs);
	}
	return loadTileSheet(ctx, TileSheetSpace::Background, 0, TilesheetAddr, PaletteAddr);
}

ox::Error loadTileSheet(Context *ctx,
                        TileSheetSpace,
                        int section,
                        ox::FileAddress tilesheetAddr,
                        ox::FileAddress paletteAddr) {
	auto [tsStat, tsStatErr] = ctx->rom->stat(tilesheetAddr);
	oxReturnError(tsStatErr);
	auto [ts, tserr] = ctx->rom->read(tilesheetAddr);
	oxReturnError(tserr);
	GbaTileMapTarget target;
	target.pal.palette = &MEM_PALLETE_BG[section];
	target.bgCtl = &bgCtl(section);
	target.tileMap = reinterpret_cast<uint16_t*>(&TILE_ADDR[section][0]);
	oxReturnError(ox::readMC(ts, tsStat.size, &target));
	// load external palette if available
	if (paletteAddr) {
		auto [palStat, palStatErr] = ctx->rom->stat(paletteAddr);
		oxReturnError(palStatErr);
		auto [pal, palErr] = ctx->rom->read(paletteAddr);
		oxReturnError(palErr);
		oxReturnError(ox::readMC(pal, palStat.size, &target.pal));
	}

	return OxError(0);
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
