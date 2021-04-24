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

class Size {
	public:
		static constexpr auto TypeName = "net.drinkingtea.nostalgia.common.Size";
		static constexpr auto Fields = 2;
		static constexpr auto TypeVersion = 1;
		int width = 0;
		int height = 0;

		constexpr Size() noexcept = default;

		constexpr Size(int width, int height) noexcept;

		constexpr Size operator+(common::Size p) const noexcept;

		constexpr Size operator-(common::Size p) const noexcept;

		constexpr Size operator*(common::Size p) const noexcept;

		constexpr Size operator/(common::Size p) const noexcept;


		constexpr Size operator+=(common::Size p) noexcept;

		constexpr Size operator-=(common::Size p) noexcept;

		constexpr Size operator*=(common::Size p) noexcept;

		constexpr Size operator/=(common::Size p) noexcept;


		constexpr Size operator+(int i) const noexcept;

		constexpr Size operator-(int i) const noexcept;

		constexpr Size operator*(int i) const noexcept;

		constexpr Size operator/(int i) const noexcept;


		constexpr Size operator+=(int i) noexcept;

		constexpr Size operator-=(int i) noexcept;

		constexpr Size operator*=(int i) noexcept;

		constexpr Size operator/=(int i) noexcept;


		constexpr bool operator==(const Size&) const noexcept;

		constexpr bool operator!=(const Size&) const noexcept;

};

constexpr Size::Size(int width, int height) noexcept {
	this->width = width;
	this->height = height;
}

constexpr Size Size::operator+(common::Size p) const noexcept {
	p.width += width;
	p.height += height;
	return p;
}

constexpr Size Size::operator-(common::Size p) const noexcept {
	auto out = *this;
	out.width -= p.width;
	out.height -= p.height;
	return out;
}

constexpr Size Size::operator*(common::Size p) const noexcept {
	p.width *= width;
	p.height *= height;
	return p;
}

constexpr Size Size::operator/(common::Size p) const noexcept {
	auto out = *this;
	out.width /= p.width;
	out.height /= p.height;
	return out;
}

constexpr Size Size::operator+=(common::Size p) noexcept {
	width += p.width;
	height += p.height;
	return *this;
}

constexpr Size Size::operator-=(common::Size p) noexcept {
	width -= p.width;
	height -= p.height;
	return *this;
}

constexpr Size Size::operator*=(common::Size p) noexcept {
	width *= p.width;
	height *= p.height;
	return *this;
}

constexpr Size Size::operator/=(common::Size p) noexcept {
	width /= p.width;
	height /= p.height;
	return *this;
}


constexpr Size Size::operator+(int i) const noexcept {
	auto out = *this;
	out.width += i;
	out.height += i;
	return out;
}

constexpr Size Size::operator-(int i) const noexcept {
	auto out = *this;
	out.width -= i;
	out.height -= i;
	return out;
}

constexpr Size Size::operator*(int i) const noexcept {
	auto out = *this;
	out.width *= i;
	out.height *= i;
	return out;
}

constexpr Size Size::operator/(int i) const noexcept {
	auto out = *this;
	out.width /= i;
	out.height /= i;
	return out;
}

constexpr Size Size::operator+=(int i) noexcept {
	width += i;
	height += i;
	return *this;
}

constexpr Size Size::operator-=(int i) noexcept {
	width -= i;
	height -= i;
	return *this;
}

constexpr Size Size::operator*=(int i) noexcept {
	width *= i;
	height *= i;
	return *this;
}

constexpr Size Size::operator/=(int i) noexcept {
	width /= i;
	height /= i;
	return *this;
}

constexpr bool Size::operator==(const Size &p) const noexcept {
	return width == p.width && height == p.height;
}


constexpr bool Size::operator!=(const Size &p) const noexcept {
	return width != p.width || height != p.height;
}


template<typename T>
constexpr ox::Error model(T *io, Size *obj) {
	io->template setTypeInfo<Size>();
	oxReturnError(io->field("width", &obj->width));
	oxReturnError(io->field("height", &obj->height));
	return OxError(0);
}

}
