/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "bounds.hpp"

namespace nostalgia {
namespace common {

Bounds::Bounds() {
}

bool Bounds::intersects(Bounds o) const {
	return o.x2() >= X && x2() >= o.X && o.y2() >= Y && y2() >= o.Y;
}

bool Bounds::contains(int x, int y) const {
	return x >= X && y >= Y && x <= x2() && y <= y2();
}

int Bounds::x2() const {
	return X + Width;
}

int Bounds::y2() const {
	return Y + Height;
}

Point Bounds::pt1() {
	return Point(X, Y);
}

Point Bounds::pt2() {
	return Point(x2(), y2());
}

}
}
