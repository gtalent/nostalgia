/*
 * Copyright 2016-2017 gtalent2@gmail.com
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
		int X = 0;
		int Y = 0;
		int Width = 0;
		int Height = 0;

		/**
		 * Constructor
		 */
		Bounds();

		bool intersects(Bounds other) const;

		bool contains(int x, int y) const;

		int x2() const;

		int y2() const;

		Point pt1();

		Point pt2();
};

}
}
