/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstdio>

#include "../media.hpp"

namespace nostalgia::core {

uint8_t *loadRom(const char *path) {
	auto file = fopen(path, "r");
	if (file) {
		fseek(file, 0, SEEK_END);
		const auto size = ftell(file);
		rewind(file);
		auto buff = new uint8_t[size];
		fread(buff, size, 1, file);
		fclose(file);
		return buff;
	} else {
		return nullptr;
	}
}

void unloadRom(uint8_t *rom) {
	delete rom;
}

}
