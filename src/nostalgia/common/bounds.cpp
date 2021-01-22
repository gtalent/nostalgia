/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "bounds.hpp"

namespace nostalgia::common {

Bounds::Bounds(int x, int y, int w, int h) {
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
}

bool Bounds::intersects(Bounds o) const {
	return o.x2() >= x && x2() >= o.x && o.y2() >= y && y2() >= o.y;
}

bool Bounds::contains(int x, int y) const {
	return x >= this->x && y >= this->y && x <= x2() && y <= y2();
}

int Bounds::x2() const {
	return x + width;
}

int Bounds::y2() const {
	return y + height;
}

Point Bounds::pt1() {
	return Point(x, y);
}

Point Bounds::pt2() {
	return Point(x2(), y2());
}

}
