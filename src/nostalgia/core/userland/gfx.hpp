/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

#include <nostalgia/core/context.hpp>

namespace nostalgia::core::renderer {

ox::Error init(Context *ctx);

ox::Error shutdown(Context *ctx);

ox::Error loadTexture(Context *ctx, int section, void *bytes, int w, int h);

}
