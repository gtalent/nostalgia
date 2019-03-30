/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

namespace ox {

enum FsType {
	OxFS_16 = 1,
	OxFS_32 = 2,
	OxFS_64 = 3
};

enum FileType {
	FileType_NormalFile = 1,
	FileType_Directory  = 2
};

constexpr const char *toString(FileType t) {
	switch (t) {
		case FileType_NormalFile:
			return "Normal File";
		case FileType_Directory:
			return "Directory";
		default:
			return "";
	}
}

struct FileStat {
	uint64_t inode = 0;
	uint64_t links = 0;
	uint64_t size = 0;
	uint8_t  fileType = 0;
};

}
