/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/fs.hpp>

#include "consts.hpp"
#include "gfx.hpp"
#include "media.hpp"
#include "types.hpp"

namespace nostalgia::core {

ox::Error init(Context *ctx);

ox::Error run();

}
