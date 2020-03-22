/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
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

ox::Error run(ox::FileSystem *fs) {
	core::Context ctx;
	ctx.rom = fs;
	oxReturnError(core::init(&ctx));
	//Zone zone;
	//oxReturnError(zone.init(&ctx, Bounds{0, 0, 40, 40}, "/TileSheets/Charset.ng", "/Palettes/Charset.npal"));
	//zone.draw(&ctx);
	oxReturnError(core::initConsole(&ctx));
	core::puts(&ctx, 10, 9, "DOPENESS!!!");
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
