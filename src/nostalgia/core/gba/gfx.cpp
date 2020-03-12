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

#include <nostalgia/core/media.hpp>
#include <nostalgia/core/gfx.hpp>

#include "addresses.hpp"

namespace nostalgia::core {

extern char charMap[128];

#define TILE_ADDR reinterpret_cast<uint16_t*>(0x06000000)

constexpr auto GBA_TILE_COLUMNS = 32;
constexpr auto GBA_TILE_ROWS = 32;

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
	oxReturnError(io->template field<Color16>("colors", [t](auto i, Color16 *c) {
		t->palette[i] = *c;
		return OxError(0);
	}));
	return OxError(0);
}

template<typename T>
ox::Error modelRead(T *io, GbaTileMapTarget *t) {
	io->setTypeInfo("nostalgia::core::NostalgiaGraphic", NostalgiaGraphic::Fields);

	uint8_t bpp;
	int dummy;
	oxReturnError(io->field("bpp", &bpp));
	oxReturnError(io->field("rows", &dummy));
	oxReturnError(io->field("columns", &dummy));
	constexpr auto Bpp8 = 1 << 7;
	*t->bgCtl = (28 << 8) | 1;
	if (bpp == 4) {
		*t->bgCtl |= (*t->bgCtl | Bpp8) ^ Bpp8; // set to use 4 bits per pixel
	} else {
		*t->bgCtl |= Bpp8; // set to use 8 bits per pixel
	}

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
			oxPanic(OxError(1), "Looking up non-existent register");
			return REG_BG0CNT;
	}
}

// Do NOT rely on Context in the GBA version of this function.
ox::Error initConsole(Context *ctx) {
	constexpr auto TilesheetAddr = "/TileSheets/Charset.ng";
	constexpr auto PaletteAddr = "/Palettes/Charset.npal";
	if (!ctx) {
		ctx = new (ox_alloca(sizeof(Context))) Context();
		auto rom = loadRom();
		if (!rom) {
			return OxError(1);
		}
		ox::FileStore32 fs(rom, 32 * ox::units::MB);
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
	target.pal.palette = &MEM_PALETTE_BG[section];
	target.bgCtl = &bgCtl(section);
	target.tileMap = &TILE_ADDR[section * 512];
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
void setTile(Context*, int layer, int column, int row, uint8_t tile) {
	MEM_BG_MAP[28 + layer][row * GBA_TILE_COLUMNS + column] = tile;
}

}
