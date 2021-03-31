/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/fs.hpp>
#include <ox/mc/mc.hpp>

#include <nostalgia/core/media.hpp>
#include <nostalgia/core/gfx.hpp>

#include "addresses.hpp"
#include "bios.hpp"
#include "irq.hpp"
#include "gfx.hpp"

namespace nostalgia::core {

constexpr auto GbaTileColumns = 32;
constexpr auto GbaTileRows = 32;

constexpr uint16_t DispStat_irq_vblank = 1 << 3;
constexpr uint16_t DispStat_irq_hblank = 1 << 4;
constexpr uint16_t DispStat_irq_vcount = 1 << 5;

struct GbaPaletteTarget {
	static constexpr auto TypeName = NostalgiaPalette::TypeName;
	static constexpr auto Fields = NostalgiaPalette::Fields;
	static constexpr auto TypeVersion = NostalgiaPalette::TypeVersion;
	volatile uint16_t *palette = nullptr;
};

struct GbaTileMapTarget {
	static constexpr auto TypeName = NostalgiaGraphic::TypeName;
	static constexpr auto Fields = NostalgiaGraphic::Fields;
	static constexpr auto TypeVersion = NostalgiaGraphic::TypeVersion;
	volatile uint32_t *bgCtl = nullptr;
	ox::FileAddress defaultPalette;
	GbaPaletteTarget pal;
	volatile uint16_t *tileMap = nullptr;
};

template<typename T>
ox::Error modelRead(T *io, GbaPaletteTarget *t) {
	io->template setTypeInfo<GbaPaletteTarget>();
	const auto colorHandler = [t](std::size_t i, Color16 *c) {
		t->palette[i] = *c;
		return OxError(0);
	};
	return io->template field<Color16, decltype(colorHandler)>("colors", colorHandler);
}

template<typename T>
ox::Error modelRead(T *io, GbaTileMapTarget *t) {
	io->template setTypeInfo<GbaTileMapTarget>();
	uint8_t bpp;
	int dummy;
	oxReturnError(io->field("bpp", &bpp));
	oxReturnError(io->field("rows", &dummy));
	oxReturnError(io->field("columns", &dummy));
	constexpr auto Bpp8 = 1 << 7;
	if (t->bgCtl) {
		*t->bgCtl = (28 << 8) | 1;
		if (bpp == 4) {
			*t->bgCtl = *t->bgCtl | ((*t->bgCtl | Bpp8) ^ Bpp8); // set to use 4 bits per pixel
		} else {
			*t->bgCtl = *t->bgCtl | Bpp8; // set to use 8 bits per pixel
		}
	}
	oxReturnError(io->field("defaultPalette", &t->defaultPalette));
	oxReturnError(io->field("pal", &t->pal));
	uint16_t intermediate = 0;
	const auto handleTileMap = [t, &intermediate](std::size_t i, uint8_t *tile) {
		if (i & 1) { // i is odd
			intermediate |= static_cast<uint16_t>(*tile) << 8;
			t->tileMap[i / 2] = intermediate;
		} else { // i is even
			intermediate = *tile & 0x00ff;
		}
		return OxError(0);
	};
	return io->template field<uint8_t, decltype(handleTileMap)>("tileMap", handleTileMap);
}

ox::Error initGfx(Context*) {
	REG_DISPCTL = DispCtl_Mode0
	            | DispCtl_SpriteMap1D
	            | DispCtl_Obj;
	// tell display to trigger vblank interrupts
	REG_DISPSTAT = REG_DISPSTAT | DispStat_irq_vblank;
	// enable vblank interrupt
	REG_IE = REG_IE | Int_vblank;
	return OxError(0);
}

ox::Error shutdownGfx(Context*) {
	return OxError(0);
}

int getScreenWidth(Context*) {
	return 240;
}

int getScreenHeight(Context*) {
	return 160;
}

common::Point getScreenSize(Context*) {
	return {240, 160};
}

uint8_t bgStatus(Context*) {
	return (REG_DISPCTL >> 8) & 0b1111;
}

void setBgStatus(Context*, uint32_t status) {
	constexpr auto Bg0Status = 8;
	REG_DISPCTL = (REG_DISPCTL & ~0b111100000000u) | status << Bg0Status;
}

bool bgStatus(Context*, unsigned bg) {
	return (REG_DISPCTL >> (8 + bg)) & 1;
}

void setBgStatus(Context*, unsigned bg, bool status) {
	constexpr auto Bg0Status = 8;
	const auto mask = static_cast<uint32_t>(status) << (Bg0Status + bg);
	REG_DISPCTL = REG_DISPCTL | ((REG_DISPCTL & ~mask) | mask);
}

[[nodiscard]] constexpr volatile uint32_t &bgCtl(int bg) noexcept {
	switch (bg) {
		case 0:
			return REG_BG0CTL;
		case 1:
			return REG_BG1CTL;
		case 2:
			return REG_BG2CTL;
		case 3:
			return REG_BG3CTL;
		default:
			oxPanic(OxError(1), "Looking up non-existent register");
			return REG_BG0CTL;
	}
}

// Do NOT rely on Context in the GBA version of this function.
ox::Error initConsole(Context *ctx) {
	constexpr auto TilesheetAddr = "/TileSheets/Charset.ng";
	constexpr auto PaletteAddr = "/Palettes/Charset.npal";
	setBgStatus(ctx, 0b0001);
	if (!ctx) {
		ctx = new (ox_alloca(sizeof(Context))) Context();
		auto rom = loadRom();
		if (!rom) {
			return OxError(1);
		}
		ox::FileStore32 fs(rom, 32 * ox::units::MB);
		ctx->rom = new (ox_alloca(sizeof(ox::FileSystem32))) ox::FileSystem32(fs);
	}
	return loadBgTileSheet(ctx, 0, TilesheetAddr, PaletteAddr);
}

ox::Error loadBgTileSheet(Context *ctx,
                          int section,
                          ox::FileAddress tilesheetAddr,
                          ox::FileAddress paletteAddr) {
	const auto [tsStat, tsStatErr] = ctx->rom->stat(tilesheetAddr);
	oxReturnError(tsStatErr);
	const auto [ts, tserr] = ctx->rom->read(tilesheetAddr);
	oxReturnError(tserr);
	GbaTileMapTarget target;
	target.pal.palette = &MEM_BG_PALETTE[section];
	target.bgCtl = &bgCtl(section);
	target.tileMap = &ox::bit_cast<uint16_t*>(MEM_BG_TILES)[section * 512];
	oxReturnError(ox::readMC(ts, tsStat.size, &target));
	// load external palette if available
	if (paletteAddr) {
		const auto [palStat, palStatErr] = ctx->rom->stat(paletteAddr);
		oxReturnError(palStatErr);
		const auto [pal, palErr] = ctx->rom->read(paletteAddr);
		oxReturnError(palErr);
		oxReturnError(ox::readMC(pal, palStat.size, &target.pal));
	}
	return OxError(0);
}

ox::Error loadSpriteTileSheet(Context *ctx,
                              int section,
                              ox::FileAddress tilesheetAddr,
                              ox::FileAddress paletteAddr) {
	const auto [tsStat, tsStatErr] = ctx->rom->stat(tilesheetAddr);
	oxReturnError(tsStatErr);
	const auto [ts, tserr] = ctx->rom->read(tilesheetAddr);
	oxReturnError(tserr);
	GbaTileMapTarget target;
	target.pal.palette = &MEM_SPRITE_PALETTE[section];
	// Is this needed? Should this be written to an equivalent sprite value?
	// target.bgCtl = &bgCtl(section);
	target.tileMap = &ox::bit_cast<uint16_t*>(MEM_SPRITE_TILES)[section * 512];
	oxReturnError(ox::readMC(ts, tsStat.size, &target));
	// load external palette if available
	if (paletteAddr) {
		const auto [palStat, palStatErr] = ctx->rom->stat(paletteAddr);
		oxReturnError(palStatErr);
		const auto [pal, palErr] = ctx->rom->read(paletteAddr);
		oxReturnError(palErr);
		oxReturnError(ox::readMC(pal, palStat.size, &target.pal));
	}
	return OxError(0);
}

ox::Error loadBgPalette(Context *ctx, int section, ox::FileAddress paletteAddr) {
	GbaPaletteTarget target;
	target.palette = &MEM_BG_PALETTE[section];
	const auto [palStat, palStatErr] = ctx->rom->stat(paletteAddr);
	oxReturnError(palStatErr);
	const auto [pal, palErr] = ctx->rom->read(paletteAddr);
	oxReturnError(palErr);
	oxReturnError(ox::readMC(pal, palStat.size, &target));
	return OxError(0);
}

ox::Error loadSpritePalette(Context *ctx, int section, ox::FileAddress paletteAddr) {
	GbaPaletteTarget target;
	target.palette = &MEM_SPRITE_PALETTE[section];
	const auto [palStat, palStatErr] = ctx->rom->stat(paletteAddr);
	oxReturnError(palStatErr);
	const auto [pal, palErr] = ctx->rom->read(paletteAddr);
	oxReturnError(palErr);
	oxReturnError(ox::readMC(pal, palStat.size, &target));
	return OxError(0);
}

// Do NOT use Context in the GBA version of this function.
void puts(Context *ctx, int column, int row, const char *str) {
	for (int i = 0; str[i]; i++) {
		setTile(ctx, 0, column + i, row, static_cast<uint8_t>(charMap[static_cast<int>(str[i])]));
	}
}

void setTile(Context*, int layer, int column, int row, uint8_t tile) {
	MEM_BG_MAP[layer][row * GbaTileColumns + column] = tile;
}

// Do NOT use Context in the GBA version of this function.
void clearTileLayer(Context*, int layer) {
	memset(&MEM_BG_MAP[layer], 0, GbaTileRows * GbaTileColumns);
}

[[maybe_unused]]
void hideSprite(Context*, unsigned idx) {
	oxAssert(g_spriteUpdates < config::GbaSpriteBufferLen, "Sprite update buffer overflow");
	GbaSpriteAttrUpdate oa;
	oa.attr0 = 2 << 8;
	oa.idx = idx;
	// block until g_spriteUpdates is less than buffer len
	if (g_spriteUpdates >= config::GbaSpriteBufferLen) {
		nostalgia_core_vblankintrwait();
	}
	if constexpr(config::GbaEventLoopTimerBased) {
		REG_IE = REG_IE & ~Int_vblank; // disable vblank interrupt handler
		g_spriteBuffer[g_spriteUpdates] = oa;
		REG_IE = REG_IE | Int_vblank; // enable vblank interrupt handler
	} else {
		const auto ie = REG_IE; // disable vblank interrupt handler
		REG_IE = REG_IE & ~Int_vblank; // disable vblank interrupt handler
		g_spriteBuffer[g_spriteUpdates] = oa;
		REG_IE = ie; // enable vblank interrupt handler
	}
	g_spriteUpdates = g_spriteUpdates + 1;
}

void setSprite(Context*,
               unsigned idx,
               unsigned x,
               unsigned y,
               unsigned tileIdx,
               unsigned spriteShape,
               unsigned spriteSize,
               unsigned flipX) {
	oxAssert(g_spriteUpdates < config::GbaSpriteBufferLen, "Sprite update buffer overflow");
	GbaSpriteAttrUpdate oa;
	oa.attr0 = static_cast<uint16_t>(y & ox::onMask<uint8_t>(7))
	         | (static_cast<uint16_t>(1) << 10) // enable alpha
				| (static_cast<uint16_t>(spriteShape) << 14);
	oa.attr1 = (static_cast<uint16_t>(x) & ox::onMask<uint8_t>(8))
	         | (static_cast<uint16_t>(flipX) << 12)
	         | (static_cast<uint16_t>(spriteSize) << 14);
	oa.attr2 = static_cast<uint16_t>(tileIdx & ox::onMask<uint16_t>(8));
	oa.idx = idx;
	// block until g_spriteUpdates is less than buffer len
	if (g_spriteUpdates >= config::GbaSpriteBufferLen) {
		nostalgia_core_vblankintrwait();
	}
	if constexpr(config::GbaEventLoopTimerBased) {
		REG_IE = REG_IE & ~Int_vblank; // disable vblank interrupt handler
		g_spriteBuffer[g_spriteUpdates] = oa;
		REG_IE = REG_IE | Int_vblank; // enable vblank interrupt handler
	} else {
		const auto ie = REG_IE; // disable vblank interrupt handler
		REG_IE = REG_IE & ~Int_vblank; // disable vblank interrupt handler
		g_spriteBuffer[g_spriteUpdates] = oa;
		REG_IE = ie; // enable vblank interrupt handler
	}
	g_spriteUpdates = g_spriteUpdates + 1;
}

}
