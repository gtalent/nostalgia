/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/fs.hpp>

namespace nostalgia::core {

ox::Result<ox::UniquePtr<ox::FileSystem>> loadRomFs(const char *path) noexcept;

ox::Result<char*> loadRom(const char *path = "") noexcept;

void unloadRom(char*) noexcept;

}
