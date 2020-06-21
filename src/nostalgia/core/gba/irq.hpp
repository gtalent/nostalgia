/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <nostalgia/core/context.hpp>

#include "addresses.hpp"

namespace nostalgia::core {

constexpr uint16_t IntId_vblank = static_cast<uint64_t>(1) << 0;
constexpr uint16_t IntId_hblank = static_cast<uint64_t>(1) << 1;
constexpr uint16_t IntId_vcount = static_cast<uint64_t>(1) << 2;
constexpr uint16_t IntId_timer0 = static_cast<uint64_t>(1) << 3;
constexpr uint16_t IntId_timer1 = static_cast<uint64_t>(1) << 4;
constexpr uint16_t IntId_timer2 = static_cast<uint64_t>(1) << 5;
constexpr uint16_t IntId_serial = static_cast<uint64_t>(1) << 6;  // link cable
constexpr uint16_t IntId_input  = static_cast<uint64_t>(1) << 14; // gamepad
constexpr uint16_t IntId_cart   = static_cast<uint64_t>(1) << 15; // cartridge removed

[[nodiscard]] ox::Error initIrq(Context *ctx);

}
