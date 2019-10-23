/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/units.hpp>
#include <nostalgia/world/world.hpp>

using namespace nostalgia::common;
using namespace nostalgia::core;
using namespace nostalgia::world;

int run(ox::FileSystem *fs) {
	Context ctx;
	init(&ctx);
	ctx.rom = fs;
	Zone zone(&ctx, Bounds{0, 0, 40, 40}, "/TileSheets/GeneralWorld");
	zone.draw(&ctx);
	run();
	return 0;
}

#ifndef OX_USE_STDLIB

extern "C" void _start() {
	ox::FileSystem32 fs(ox::FileStore32(loadRom(), 32 * ox::units::MB));
	run(&fs);
}

#else

int main() {
	return run(nullptr);
}

#endif
