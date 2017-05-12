/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "point.hpp"

namespace nostalgia {
namespace common {

Point::Point() {
}

Point::Point(int x, int y) {
	X = x;
	Y = y;
}

Point Point::operator+(common::Point p) const {
	p.X += X;
	p.Y += Y;
	return p;
}

Point Point::operator-(common::Point p) const {
	auto out = *this; 
	out.X -= p.X;
	out.Y -= p.Y;
	return out;
}

Point Point::operator*(common::Point p) const {
	p.X *= X;
	p.Y *= Y;
	return p;
}

Point Point::operator/(common::Point p) const {
	auto out = *this; 
	out.X /= p.X;
	out.Y /= p.Y;
	return out;
}

Point Point::operator+=(common::Point p) {
	X += p.X;
	Y += p.Y;
	return *this;
}

Point Point::operator-=(common::Point p) {
	X -= p.X;
	Y -= p.Y;
	return *this;
}

Point Point::operator*=(common::Point p) {
	X *= p.X;
	Y *= p.Y;
	return *this;
}

Point Point::operator/=(common::Point p) {
	X /= p.X;
	Y /= p.Y;
	return *this;
}


Point Point::operator+(int i) const {
	auto out = *this; 
	out.X += i;
	out.Y += i;
	return out;
}

Point Point::operator-(int i) const {
	auto out = *this; 
	out.X -= i;
	out.Y -= i;
	return out;
}

Point Point::operator*(int i) const {
	auto out = *this; 
	out.X *= i;
	out.Y *= i;
	return out;
}

Point Point::operator/(int i) const {
	auto out = *this; 
	out.X /= i;
	out.Y /= i;
	return out;
}

Point Point::operator+=(int i) {
	X += i;
	Y += i;
	return *this;
}

Point Point::operator-=(int i) {
	X -= i;
	Y -= i;
	return *this;
}

Point Point::operator*=(int i) {
	X *= i;
	Y *= i;
	return *this;
}

Point Point::operator/=(int i) {
	X /= i;
	Y /= i;
	return *this;
}

}
}
