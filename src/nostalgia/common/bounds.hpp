/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "point.hpp"

namespace nostalgia::common {

class Bounds {

	public:
		static constexpr auto TypeName = "net.drinkingtea.nostalgia.common.Bounds";
		static constexpr auto Fields = 4;
		static constexpr auto TypeVersion = 1;
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		Bounds() = default;

		Bounds(int x, int y, int w, int h);

		[[nodiscard]] bool intersects(Bounds other) const;

		[[nodiscard]] bool contains(int x, int y) const;

		[[nodiscard]] int x2() const;

		[[nodiscard]] int y2() const;

		[[nodiscard]] Point pt1();

		[[nodiscard]] Point pt2();

};

template<typename T>
ox::Error model(T *io, Bounds *obj) {
	io->template setTypeInfo<Point>();
	oxReturnError(io->field("x", &obj->x));
	oxReturnError(io->field("y", &obj->y));
	oxReturnError(io->field("width", &obj->width));
	oxReturnError(io->field("height", &obj->height));
	return OxError(0);
}

}
