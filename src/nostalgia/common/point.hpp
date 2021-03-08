/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>

namespace nostalgia::common {

class Point {
	public:
		int x = 0;
		int y = 0;

		constexpr Point() = default;

		constexpr Point(int x, int y);

		constexpr Point operator+(common::Point p) const;

		constexpr Point operator-(common::Point p) const;

		constexpr Point operator*(common::Point p) const;

		constexpr Point operator/(common::Point p) const;


		constexpr Point operator+=(common::Point p);

		constexpr Point operator-=(common::Point p);

		constexpr Point operator*=(common::Point p);

		constexpr Point operator/=(common::Point p);


		constexpr Point operator+(int i) const;

		constexpr Point operator-(int i) const;

		constexpr Point operator*(int i) const;

		constexpr Point operator/(int i) const;


		constexpr Point operator+=(int i);

		constexpr Point operator-=(int i);

		constexpr Point operator*=(int i);

		constexpr Point operator/=(int i);


		constexpr bool operator==(const Point&) const;

		constexpr bool operator!=(const Point&) const;

};

constexpr Point::Point(int x, int y) {
	this->x = x;
	this->y = y;
}

constexpr Point Point::operator+(common::Point p) const {
	p.x += x;
	p.y += y;
	return p;
}

constexpr Point Point::operator-(common::Point p) const {
	auto out = *this;
	out.x -= p.x;
	out.y -= p.y;
	return out;
}

constexpr Point Point::operator*(common::Point p) const {
	p.x *= x;
	p.y *= y;
	return p;
}

constexpr Point Point::operator/(common::Point p) const {
	auto out = *this;
	out.x /= p.x;
	out.y /= p.y;
	return out;
}

constexpr Point Point::operator+=(common::Point p) {
	x += p.x;
	y += p.y;
	return *this;
}

constexpr Point Point::operator-=(common::Point p) {
	x -= p.x;
	y -= p.y;
	return *this;
}

constexpr Point Point::operator*=(common::Point p) {
	x *= p.x;
	y *= p.y;
	return *this;
}

constexpr Point Point::operator/=(common::Point p) {
	x /= p.x;
	y /= p.y;
	return *this;
}


constexpr Point Point::operator+(int i) const {
	auto out = *this;
	out.x += i;
	out.y += i;
	return out;
}

constexpr Point Point::operator-(int i) const {
	auto out = *this;
	out.x -= i;
	out.y -= i;
	return out;
}

constexpr Point Point::operator*(int i) const {
	auto out = *this;
	out.x *= i;
	out.y *= i;
	return out;
}

constexpr Point Point::operator/(int i) const {
	auto out = *this;
	out.x /= i;
	out.y /= i;
	return out;
}

constexpr Point Point::operator+=(int i) {
	x += i;
	y += i;
	return *this;
}

constexpr Point Point::operator-=(int i) {
	x -= i;
	y -= i;
	return *this;
}

constexpr Point Point::operator*=(int i) {
	x *= i;
	y *= i;
	return *this;
}

constexpr Point Point::operator/=(int i) {
	x /= i;
	y /= i;
	return *this;
}

constexpr bool Point::operator==(const Point &p) const {
	return x == p.x && y == p.y;
}


constexpr bool Point::operator!=(const Point &p) const {
	return x != p.x || y != p.y;
}


template<typename T>
ox::Error model(T *io, Point *obj) {
	io->setTypeInfo("nostalgia::common::Bounds", 2);
	oxReturnError(io->field("x", &obj->x));
	oxReturnError(io->field("y", &obj->y));
	return OxError(0);
}

}
