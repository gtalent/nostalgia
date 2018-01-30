/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
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
	this->x = x;
	this->y = y;
}

Point Point::operator+(common::Point p) const {
	p.x += x;
	p.y += y;
	return p;
}

Point Point::operator-(common::Point p) const {
	auto out = *this; 
	out.x -= p.x;
	out.y -= p.y;
	return out;
}

Point Point::operator*(common::Point p) const {
	p.x *= x;
	p.y *= y;
	return p;
}

Point Point::operator/(common::Point p) const {
	auto out = *this; 
	out.x /= p.x;
	out.y /= p.y;
	return out;
}

Point Point::operator+=(common::Point p) {
	x += p.x;
	y += p.y;
	return *this;
}

Point Point::operator-=(common::Point p) {
	x -= p.x;
	y -= p.y;
	return *this;
}

Point Point::operator*=(common::Point p) {
	x *= p.x;
	y *= p.y;
	return *this;
}

Point Point::operator/=(common::Point p) {
	x /= p.x;
	y /= p.y;
	return *this;
}


Point Point::operator+(int i) const {
	auto out = *this; 
	out.x += i;
	out.y += i;
	return out;
}

Point Point::operator-(int i) const {
	auto out = *this; 
	out.x -= i;
	out.y -= i;
	return out;
}

Point Point::operator*(int i) const {
	auto out = *this; 
	out.x *= i;
	out.y *= i;
	return out;
}

Point Point::operator/(int i) const {
	auto out = *this; 
	out.x /= i;
	out.y /= i;
	return out;
}

Point Point::operator+=(int i) {
	x += i;
	y += i;
	return *this;
}

Point Point::operator-=(int i) {
	x -= i;
	y -= i;
	return *this;
}

Point Point::operator*=(int i) {
	x *= i;
	y *= i;
	return *this;
}

Point Point::operator/=(int i) {
	x /= i;
	y /= i;
	return *this;
}

}
}
