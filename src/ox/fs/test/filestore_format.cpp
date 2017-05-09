/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <ox/fs/filestore.hpp>

using namespace ox::fs;

int main() {
	const auto size = 65535;
	uint8_t volume[size];
	uint32_t err = 0;
	FileStore32::format(volume, size);
	return err;
}
