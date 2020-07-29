/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
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

namespace nostalgia::core {

using event_handler = int(*)(Context*);

[[nodiscard]] ox::Error init(Context *ctx);

[[nodiscard]] ox::Error run(Context *ctx);

// Sets event handler that sleeps for the time given in the return value. The
// sleep time is a minimum of ~16 milliseconds.
void setEventHandler(event_handler);

// Returns the number of milliseconds that have passed since the start of the
//  program.
[[nodiscard]] uint64_t ticksMs();

}
