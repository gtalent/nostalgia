/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <fstream>

#include <ox/std/trace.hpp>

#include "../media.hpp"

namespace nostalgia::core {

char *loadRom(const char *path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file.good()) {
		oxTrace("nostalgia::core::userland::loadRom") << "Read failed:" << path;
		return nullptr;
	}

	const std::size_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	auto buff = new char[size];
	file.read(buff, size);
	return buff;
}

void unloadRom(char *rom) {
	delete rom;
}

}
