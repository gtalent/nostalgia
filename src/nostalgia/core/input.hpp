/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace nostalgia::core {

enum Key {
	// GBA implementation currently relies on GamePad entry order
	GamePad_A      =   1,
	GamePad_B      =   2,
	GamePad_Select =   4,
	GamePad_Start  =   8,
	GamePad_Right  =  16,
	GamePad_Left   =  32,
	GamePad_Up     =  64,
	GamePad_Down   = 128,
	GamePad_R      = 256,
	GamePad_L      = 512,
};

[[nodiscard]]
bool buttonDown(Key) noexcept;

}
