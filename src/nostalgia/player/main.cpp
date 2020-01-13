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
	oxReturnError(core::run());
	oxReturnError(core::shutdownGfx());
	return OxError(0);
}

int main(int argc, const char **argv) {
	if (argc > 1) {
		ox::FileSystem *fs = nullptr;
		uint8_t *rom = nullptr;
		auto path = argv[1];
		const auto lastDot = ox_lastIndexOf(path, '.');
		const auto fsExt = lastDot != -1 ? path + lastDot : "";
		if (ox_strcmp(fsExt, ".oxfs") == 0) {
			rom = core::loadRom(path);
			if (!rom) {
				return 1;
			}
			fs = new (ox_alloca(sizeof(ox::FileStore32))) ox::FileSystem32(ox::FileStore32(rom, 32 * ox::units::MB));
		} else {
#ifdef OX_HAS_PASSTHROUGHFS
			fs = new (ox_alloca(sizeof(ox::PassThroughFS))) ox::PassThroughFS(path);
#else
			return 2;
#endif
		}
		auto err = run(fs);
		oxAssert(err, "Something went wrong...");
		fs->~FileSystem();
		core::unloadRom(rom);
		return err;
	}
	return 3;
}
