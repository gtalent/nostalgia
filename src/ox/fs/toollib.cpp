/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <string.h>

#include "toollib.hpp"

uint8_t *loadFileBuff(FILE *file, ::size_t *sizeOut) {
	if (file) {
		fseek(file, 0, SEEK_END);
		const auto size = ftell(file);
		rewind(file);
		auto buff = new uint8_t[size];
		auto itemsRead = fread(buff, size, 1, file);
		fclose(file);
		if (sizeOut) {
			*sizeOut = itemsRead ? size : 0;
		}
		return buff;
	} else {
		return nullptr;
	}
}

uint8_t *loadFileBuff(const char *path, ::size_t *sizeOut) {
	return loadFileBuff(fopen(path, "rb"), sizeOut);
}
