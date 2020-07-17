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
#include <nostalgia/world/world.hpp>

using namespace nostalgia;

static unsigned spriteX = 0;

static int mainLoop() {
	spriteX += 2;
	spriteX &= 255;
	constexpr auto y = 64;
	core::setSprite(0, spriteX +  8, y, 'n' - ('a' - 1));
	core::setSprite(1, spriteX + 16, y, 'o' - ('a' - 1));
	core::setSprite(2, spriteX + 24, y, 's' - ('a' - 1));
	core::setSprite(3, spriteX + 32, y, 't' - ('a' - 1));
	core::setSprite(4, spriteX + 40, y, 'a' - ('a' - 1));
	core::setSprite(5, spriteX + 48, y, 'l' - ('a' - 1));
	core::setSprite(6, spriteX + 56, y, 'g' - ('a' - 1));
	core::setSprite(7, spriteX + 64, y, 'i' - ('a' - 1));
	core::setSprite(8, spriteX + 72, y, 'a' - ('a' - 1));
	return 16;
}

ox::Error run(ox::FileSystem *fs) {
	core::Context ctx;
	ctx.rom = fs;
	oxReturnError(core::init(&ctx));
	//Zone zone;
	//oxReturnError(zone.init(&ctx, Bounds{0, 0, 40, 40}, "/TileSheets/Charset.ng", "/Palettes/Charset.npal"));
	//zone.draw(&ctx);
	constexpr auto TileSheetAddr = "/TileSheets/Charset.ng";
	constexpr auto PaletteAddr = "/Palettes/Charset.npal";
	oxReturnError(core::loadSpriteTileSheet(&ctx, 0, TileSheetAddr, PaletteAddr));
	oxReturnError(core::initConsole(&ctx));
	core::puts(&ctx, 10, 9, "DOPENESS!!!");
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
