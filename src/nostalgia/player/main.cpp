/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/fs.hpp>
#include <ox/std/units.hpp>
#include <nostalgia/core/core.hpp>
#include <nostalgia/core/input.hpp>
#include <nostalgia/world/world.hpp>

using namespace nostalgia;

static unsigned spriteX = 72;
static unsigned spriteY = 64;

static void createCloud(core::Context *ctx, int x, int y) {
	core::setTile(ctx, 0, x + 0, y + 0, 8);
	core::setTile(ctx, 0, x + 1, y + 0, 9);
	core::setTile(ctx, 0, x + 0, y + 1, 10);
	core::setTile(ctx, 0, x + 1, y + 1, 11);
}

static ox::Error initArena(core::Context *ctx) {
	constexpr auto TileSheetAddr = "/TileSheets/Background.ng";
	constexpr auto PaletteAddr = "/Palettes/Background.npal";
	oxReturnError(core::loadBgTileSheet(ctx, 0, TileSheetAddr, PaletteAddr));
	for (int ii = 0; ii < 16; ii += 2) {
		for (int i = 0; i < 32; i += 2) {
			core::setTile(ctx, 0, i + 0, ii + 0, 4);
			core::setTile(ctx, 0, i + 1, ii + 0, 5);
			core::setTile(ctx, 0, i + 0, ii + 1, 6);
			core::setTile(ctx, 0, i + 1, ii + 1, 7);
		}
	}
	createCloud(ctx, 8, 10);
	createCloud(ctx, 22, 7);
	createCloud(ctx, 3, 4);
	createCloud(ctx, 16, 2);
	return OxError(0);
}

static int mainLoop() {
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
	//core::setSprite(0, spriteX +  8, spriteY, 'n' - ('a' - 1));
	return 16;
}

ox::Error run(ox::FileSystem *fs) {
	core::Context ctx;
	ctx.rom = fs;
	oxReturnError(core::init(&ctx));
	//Zone zone;
	//oxReturnError(zone.init(&ctx, Bounds{0, 0, 40, 40}, "/TileSheets/Charset.ng", "/Palettes/Charset.npal"));
	//zone.draw(&ctx);
	oxReturnError(initArena(&ctx));
	core::setEventHandler(mainLoop);
	oxReturnError(core::run(&ctx));
	oxReturnError(core::shutdownGfx(&ctx));
	return OxError(0);
}

int main(int argc, const char **argv) {
	if (argc > 1) {
		auto path = argv[1];
		auto fs = core::loadRomFs(path);
		auto err = run(fs);
		oxAssert(err, "Something went wrong...");
		delete fs;
		return err;
	}
	return 1;
}
