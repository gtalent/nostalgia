/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>

namespace nostalgia {
namespace common {

class Point {
	public:
		int x = 0;
		int y = 0;

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

template<typename T>
ox::Error ioOp(T *io, Point *obj) {
	ox::Error err = 0;
	io->setFields(2);
	err |= io->op("x", &obj->x);
	err |= io->op("y", &obj->y);
	return err;
}

}
}
