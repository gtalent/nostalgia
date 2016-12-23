/*
 * Copyright 2016 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef NOSTALGIA_COMMON_POINT_HPP
#define NOSTALGIA_COMMON_POINT_HPP

namespace wombat {
namespace common {

class Point {
	public:
		int X = 0;
		int Y = 0;

		Point();

		Point(int x, int y);

		Point operator+(common::Point p) const;

		Point operator-(common::Point p) const;

		Point operator*(common::Point p) const;

		Point operator/(common::Point p) const;


		Point operator+=(common::Point p);

		Point operator-=(common::Point p);

		Point operator*=(common::Point p);

		Point operator/=(common::Point p);


		Point operator+(int i) const;

		Point operator-(int i) const;

		Point operator*(int i) const;

		Point operator/(int i) const;


		Point operator+=(int i);

		Point operator-=(int i);

		Point operator*=(int i);

		Point operator/=(int i);
};

}
}

#endif
