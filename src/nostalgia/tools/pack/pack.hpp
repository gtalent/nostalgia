/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#include <ox/fs/fs.hpp>

namespace nostalgia {

ox::Error pack(ox::PassThroughFS *src, ox::FileSystem32 *dest, std::string path = "/");

}
