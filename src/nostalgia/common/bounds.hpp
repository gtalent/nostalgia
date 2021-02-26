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
	auto err = OxError(0);
	io->setTypeInfo("nostalgia::common::Bounds", 4);
	err |= io->field("x", &obj->x);
	err |= io->field("y", &obj->y);
	err |= io->field("width", &obj->width);
	err |= io->field("height", &obj->height);
	return err;
}

}
