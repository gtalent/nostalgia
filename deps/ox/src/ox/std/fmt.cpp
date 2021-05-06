/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "fmt.hpp"

namespace ox::detail {

static_assert(argCount("sadf asdf") == 0);
static_assert(argCount("{}") == 1);
static_assert(argCount("{}{}") == 2);
static_assert(argCount("thing1: {}, thing2: {}") == 2);
static_assert(argCount("thing1: {}, thing2: {}{}") == 3);

static_assert([] {
		constexpr auto fmt = "{}";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<2>{
			{
				{"", 0},
				{"", 0},
			}
		};
	}()
);
static_assert([] {
		constexpr auto fmt = "thing: {}";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<2>{
			{
				{"thing: ", 7},
				{"", 0},
			}
		};
	}()
);
static_assert([] {
		constexpr auto fmt = "thing: {}, thing2: {}";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<3>{
			{
				{"thing: ", 7},
				{", thing2: ", 10},
				{"", 0},
			}
		};
	}()
);
static_assert([] {
		constexpr auto fmt = "thing: {}, thing2: {}s";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<3>{
			{
				{"thing: ", 7},
				{", thing2: ", 10},
				{"s", 1},
			}
		};
	}()
);
static_assert([] {
		constexpr auto fmt = "loadTexture: section: {}, w: {}, h: {}";
		return fmtSegments<argCount(fmt) + 1>(fmt) == Fmt<4>{
			{
				{"loadTexture: section: ", 22},
				{", w: ", 5},
				{", h: ", 5},
			}
		};
	}()
);


}
