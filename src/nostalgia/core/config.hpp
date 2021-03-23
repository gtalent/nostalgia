/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

namespace nostalgia::core {

namespace config {

enum class SdlVsync {
	Adaptive = -1,
	Off = 0,
	On = 1,
};

constexpr auto GbaSpriteBufferLen = 128;
constexpr auto GbaEventLoopTimerBased = false;
constexpr auto UserlandFpsPrint = false;
constexpr auto SdlVsyncOption = static_cast<int>(SdlVsync::Adaptive);

}

using gba_timer_t = uint32_t;

}
