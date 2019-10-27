/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/fs.hpp>
#include <ox/std/units.hpp>
#include <nostalgia/world/world.hpp>

using namespace nostalgia::common;
using namespace nostalgia::core;
using namespace nostalgia::world;

ox::Error run(ox::FileSystem *fs) {
	Context ctx;
	oxReturnError(init(&ctx));
	ctx.rom = fs;
	Zone zone;
	oxReturnError(zone.init(&ctx, Bounds{0, 0, 40, 40}, "/TileSheets/Charset.ng", "/Palettes/Charset.npal"));
	zone.draw(&ctx);
	oxReturnError(run());
	oxReturnError(shutdownGfx());
	return OxError(0);
}

#ifndef OX_USE_STDLIB

extern "C" void _start() {
	ox::FileSystem32 fs(ox::FileStore32(loadRom(), 32 * ox::units::MB));
	run(&fs);
}

#else

int main(int argc, const char **argv) {
	if (argc > 1) {
		ox::PassThroughFS fs(argv[1]);
		auto err = run(&fs);
		oxAssert(err, "Something went wrong...");
		return err;
	}
	return 2;
}

#endif
