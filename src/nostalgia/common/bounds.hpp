/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "point.hpp"

namespace nostalgia {
namespace common {

class Bounds {
	public:
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		Bounds() = default;

		Bounds(int x, int y, int w, int h);

		bool intersects(Bounds other) const;

		bool contains(int x, int y) const;

		int x2() const;

		int y2() const;

		Point pt1();

		Point pt2();
};

template<typename T>
ox::Error ioOp(T *io, Bounds *obj) {
	ox::Error err = 0;
	io->setFields(4);
	err |= io->op("x", &obj->x);
	err |= io->op("y", &obj->y);
	err |= io->op("width", &obj->width);
	err |= io->op("height", &obj->height);
	return err;
}

}
}
