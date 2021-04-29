/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <nostalgia/core/core.hpp>

using namespace nostalgia;

static unsigned spriteX = 72;
static unsigned spriteY = 64;

static int eventHandler(core::Context *ctx) noexcept {
	if (core::buttonDown(core::GamePad_Right)) {
		spriteX += 2;
	} else if (core::buttonDown(core::GamePad_Left)) {
		spriteX -= 2;
	}
	if (core::buttonDown(core::GamePad_Down)) {
		spriteY += 2;
	} else if (core::buttonDown(core::GamePad_Up)) {
		spriteY -= 2;
	}
	constexpr auto s = "nostalgia";
	for (unsigned i = 0; s[i]; ++i) {
		const auto c = static_cast<unsigned>(s[i] - ('a' - 1));
		core::setSprite(ctx, i, spriteX + 8 * (i + 1), spriteY, c);
	}
	return 16;
}

ox::Error run(ox::FileSystem *fs) noexcept {
	core::Context ctx;
	ctx.rom = fs;
	oxReturnError(core::init(&ctx));
	constexpr auto TileSheetAddr = "/TileSheets/Charset.ng";
	constexpr auto PaletteAddr = "/Palettes/Charset.npal";
	oxReturnError(core::loadSpriteTileSheet(&ctx, 0, TileSheetAddr, PaletteAddr));
	oxReturnError(core::initConsole(&ctx));
	core::puts(&ctx, 10, 9, "DOPENESS!!!");
	core::setEventHandler(&ctx, eventHandler);
	oxReturnError(core::run(&ctx));
	oxReturnError(core::shutdownGfx(&ctx));
	return OxError(0);
}

