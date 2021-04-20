/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "media.hpp"

namespace nostalgia::core {

ox::Result<ox::FileSystem*> loadRomFs(const char *path) noexcept {
	const auto lastDot = ox_lastIndexOf(path, '.');
	const auto fsExt = lastDot != -1 ? path + lastDot : "";
	if (ox_strcmp(fsExt, ".oxfs") == 0) {
		oxRequire(rom, core::loadRom(path));
		return new ox::FileSystem32(rom, 32 * ox::units::MB, unloadRom);
	} else {
#ifdef OX_HAS_PASSTHROUGHFS
		return new ox::PassThroughFS(path);
#else
		return OxError(2);
#endif
	}
}

}
