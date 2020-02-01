/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/fs.hpp>

#include "media.hpp"
#include "ox/std/bit.hpp"

namespace nostalgia::core {

ox::FileSystem *loadRomFs(const char *path) {
	auto rom = loadRom(path);
	return new ox::FileSystem32(rom, 32 * ox::units::MB, [](uint8_t *buff) {
		unloadRom(ox::bit_cast<char*>(buff));
	});
}

}
