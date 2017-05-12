/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdio.h>
#include <ox/std/std.hpp>
#include <ox/fs/filestore.hpp>

using namespace ox;

template<typename FileStore>
int test() {
	const uint16_t size = ~0;
	uint8_t volume[size];
	char out[6];
	typename FileStore::FsSize_t outSize;
	FileStore::format(volume, size);
	FileStore *fs = (FileStore*) volume;

	if (fs->write(1, (void*) "Hello", 6) ||
		 fs->read(1, (char*) out, &outSize) ||
		 ox_strcmp("Hello", out)) {
		printf("Failure 1\n");
		return 1;
	}

	if (fs->write(2, (void*) "World", 6) ||
		 fs->read(2, (char*) out, &outSize) ||
		 ox_strcmp("World", out)) {
		printf("Failure 2\n");
		return 2;
	}

	// make sure first value was not overwritten
	if (fs->read(1, (char*) out, &outSize) ||
		 ox_strcmp("Hello", out)) {
		printf("Failure 3\n");
		return 3;
	}

	if (fs->remove(1)) {
		printf("Failure 4\n");
		return 4;
	}

	// make sure inode is not found
	if (fs->read(1, (char*) out, &outSize) == 0) {
		printf("Failure 5\n");
		return 5;
	}

	// make sure 2 is still available
	if (fs->write(2, (void*) "World", 6) ||
		 fs->read(2, (char*) out, &outSize) ||
		 ox_strcmp("World", out)) {
		printf("Failure 6\n");
		return 6;
	}

	return 0;
}

int main() {
	return test<FileStore16>() || test<FileStore32>() | test<FileStore64>();
}
