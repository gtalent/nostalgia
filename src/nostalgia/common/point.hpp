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
		static constexpr auto TypeName = "net.drinkingtea.nostalgia.common.Point";
		static constexpr auto Fields = 2;
		static constexpr auto TypeVersion = 1;
		int x = 0;
		int y = 0;

		constexpr Point() noexcept = default;

		constexpr Point(int x, int y) noexcept;

		constexpr Point operator+(const common::Point &p) const noexcept;

		constexpr Point operator-(const common::Point &p) const noexcept;

		constexpr Point operator*(const common::Point &p) const noexcept;

		constexpr Point operator/(const common::Point &p) const noexcept;


		constexpr Point operator+=(const common::Point &p) noexcept;

		constexpr Point operator-=(const common::Point &p) noexcept;

		constexpr Point operator*=(const common::Point &p) noexcept;

		constexpr Point operator/=(const common::Point &p) noexcept;


		constexpr Point operator+(int i) const noexcept;

		constexpr Point operator-(int i) const noexcept;

		constexpr Point operator*(int i) const noexcept;

		constexpr Point operator/(int i) const noexcept;


		constexpr Point operator+=(int i) noexcept;

		constexpr Point operator-=(int i) noexcept;

		constexpr Point operator*=(int i) noexcept;

		constexpr Point operator/=(int i) noexcept;


		constexpr bool operator==(const Point&) const noexcept;

		constexpr bool operator!=(const Point&) const noexcept;

};

constexpr Point::Point(int x, int y) noexcept {
	this->x = x;
	this->y = y;
}

constexpr Point Point::operator+(const common::Point &p) const noexcept {
	auto out = *this;
	out.x += x;
	out.y += y;
	return p;
}

constexpr Point Point::operator-(const common::Point &p) const noexcept {
	auto out = *this;
	out.x -= p.x;
	out.y -= p.y;
	return out;
}

constexpr Point Point::operator*(const common::Point &p) const noexcept {
	auto out = *this;
	out.x *= x;
	out.y *= y;
	return p;
}

constexpr Point Point::operator/(const common::Point &p) const noexcept {
	auto out = *this;
	out.x /= p.x;
	out.y /= p.y;
	return out;
}

constexpr Point Point::operator+=(const common::Point &p) noexcept {
	x += p.x;
	y += p.y;
	return *this;
}

constexpr Point Point::operator-=(const common::Point &p) noexcept {
	x -= p.x;
	y -= p.y;
	return *this;
}

constexpr Point Point::operator*=(const common::Point &p) noexcept {
	x *= p.x;
	y *= p.y;
	return *this;
}

constexpr Point Point::operator/=(const common::Point &p) noexcept {
	x /= p.x;
	y /= p.y;
	return *this;
}


constexpr Point Point::operator+(int i) const noexcept {
	auto out = *this;
	out.x += i;
	out.y += i;
	return out;
}

constexpr Point Point::operator-(int i) const noexcept {
	auto out = *this;
	out.x -= i;
	out.y -= i;
	return out;
}

constexpr Point Point::operator*(int i) const noexcept {
	auto out = *this;
	out.x *= i;
	out.y *= i;
	return out;
}

constexpr Point Point::operator/(int i) const noexcept {
	auto out = *this;
	out.x /= i;
	out.y /= i;
	return out;
}

constexpr Point Point::operator+=(int i) noexcept {
	x += i;
	y += i;
	return *this;
}

constexpr Point Point::operator-=(int i) noexcept {
	x -= i;
	y -= i;
	return *this;
}

constexpr Point Point::operator*=(int i) noexcept {
	x *= i;
	y *= i;
	return *this;
}

constexpr Point Point::operator/=(int i) noexcept {
	x /= i;
	y /= i;
	return *this;
}

constexpr bool Point::operator==(const Point &p) const noexcept {
	return x == p.x && y == p.y;
}


constexpr bool Point::operator!=(const Point &p) const noexcept {
	return x != p.x || y != p.y;
}


template<typename T>
constexpr ox::Error model(T *io, Point *obj) {
	io->template setTypeInfo<Point>();
	oxReturnError(io->field("x", &obj->x));
	oxReturnError(io->field("y", &obj->y));
	return OxError(0);
}

}
