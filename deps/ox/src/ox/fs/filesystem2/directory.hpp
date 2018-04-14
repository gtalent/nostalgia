/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/fs/filesystem/pathiterator.hpp>
#include <ox/fs/filestore.hpp>

namespace ox::fs {

struct DirectoryData {
};

class Directory {

	Error add(const PathIterator &it, uint64_t inode);

	Error rm(PathIterator &it);

};

}
