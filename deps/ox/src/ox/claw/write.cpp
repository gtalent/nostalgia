/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "write.hpp"

namespace ox::detail {

struct versioned_type {
	static constexpr int TypeVersion = 4;
};

struct unversioned_type {
};

static_assert(type_version<versioned_type>::value == 4);
static_assert(type_version<unversioned_type>::value == -1);

}
