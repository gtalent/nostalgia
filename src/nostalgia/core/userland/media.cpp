/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <fstream>

#include <ox/std/trace.hpp>

#include "../media.hpp"

namespace nostalgia::core {

ox::Result<char*> loadRom(const char *path) noexcept {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.good()) {
		oxErrorf("Could not find ROM file: {}", path);
		return OxError(1, "Could not find ROM file");
	}
	try {
		const auto size = file.tellg();
		file.seekg(0, std::ios::beg);
		auto buff = new char[static_cast<std::size_t>(size)];
		file.read(buff, size);
		return buff;
	} catch (const std::ios_base::failure &e) {
		oxErrorf("Could not read ROM file: {}", e.what());
		return OxError(2, "Could not read ROM file");
	}
}

void unloadRom(char *rom) noexcept {
	delete rom;
}

}
