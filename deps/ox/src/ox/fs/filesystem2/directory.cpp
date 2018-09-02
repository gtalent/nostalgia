/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "directory.hpp"

namespace ox::fs {

template class Directory<FileStore16, uint16_t>;
template class Directory<FileStore32, uint32_t>;

template struct DirectoryEntry<uint16_t>;
template struct DirectoryEntry<uint32_t>;

}
