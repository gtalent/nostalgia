/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

namespace ox {

enum class FileType: uint8_t {
	None = 0,
	NormalFile = 1,
	Directory  = 2
};

constexpr const char *toString(FileType t) noexcept {
	switch (t) {
		case FileType::NormalFile:
			return "Normal File";
		case FileType::Directory:
			return "Directory";
		default:
			return "";
	}
}

struct FileStat {
	uint64_t inode = 0;
	uint64_t links = 0;
	uint64_t size = 0;
	FileType fileType = FileType::None;
};

}
