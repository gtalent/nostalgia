/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "filesystem.hpp"

namespace ox {

FileSystem *createFileSystem(void *buff, size_t buffSize) {
	auto version = ((FileStore16*) buff)->version();
	auto type = ((FileStore16*) buff)->fsType();
	FileSystem *fs = nullptr;

	switch (version) {
		case 5:
			switch (type) {
				case ox::OxFS_16:
					fs = new FileSystem16(buff);
					break;
				case ox::OxFS_32:
					fs = new FileSystem32(buff);
					break;
				case ox::OxFS_64:
					fs = new FileSystem64(buff);
					break;
			}
			break;
		default:
			break;
	}

	if (fs && fs->size() > buffSize) {
		delete fs;
		fs = nullptr;
	}

	return fs;
}

FileSystem *expandCopy(FileSystem *fs, size_t size) {
	auto fsBuff = fs->buff();
	FileSystem *retval = nullptr;

	if (fs->size() <= size) {
		auto cloneBuff = new uint8_t[size];
		ox_memcpy(cloneBuff, fsBuff, fs->size());

		fsBuff = cloneBuff;
		retval = createFileSystem(fsBuff, size);
		retval->resize(size);
	}

	return retval;
}

FileSystem *expandCopyCleanup(FileSystem *fs, size_t size) {
	auto out = expandCopy(fs, size);

	if (out) {
		delete[] fs->buff();
		delete fs;
	} else {
		out = fs;
	}

	return out;
}

}
