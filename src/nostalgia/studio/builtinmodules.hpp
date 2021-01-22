/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <nostalgia/core/studio/module.hpp>

namespace nostalgia::studio {

[[maybe_unused]] // GCC warns about the existence of this "unused" constexpr list in a header file...
constexpr auto BuiltinModules = {
	[] {
		return new core::Module();
	},
};

}
