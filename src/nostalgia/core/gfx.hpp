/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

#include "context.hpp"
#include "types.hpp"

namespace nostalgia::core {

ox::Error initGfx(Context *ctx);

ox::Error initConsole(Context *ctx);

ox::Error loadTileSheet(Context *ctx, InodeId_t inode);

void puts(Context *ctx, int loc, const char *str);

void setTile(Context *ctx, int layer, int column, int row, uint8_t tile);

}
