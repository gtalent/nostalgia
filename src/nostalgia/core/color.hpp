/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>
#include <nostalgia/common/point.hpp>

#include "context.hpp"

namespace nostalgia::core {

using Color16 = uint16_t;

/**
 * Nostalgia Core logically uses 16 bit colors, but must translate that to 32
 * bit colors in some implementations.
 */

using Color32 = uint32_t;

[[nodiscard]]
constexpr Color32 toColor32(Color16 nc) noexcept {
	Color32 r = static_cast<Color32>(((nc & 0b0000000000011111) >> 0) * 8);
	Color32 g = static_cast<Color32>(((nc & 0b0000001111100000) >> 5) * 8);
	Color32 b = static_cast<Color32>(((nc & 0b0111110000000000) >> 10) * 8);
	Color32 a = 255;
	return r | (g << 8) | (b << 16) | (a << 24);
}


[[nodiscard]]
constexpr uint8_t red16(Color16 c) noexcept {
	return c & 0b0000000000011111;
}

[[nodiscard]]
constexpr uint8_t green16(Color16 c) noexcept {
	return (c & 0b0000001111100000) >> 5;
}

[[nodiscard]]
constexpr uint8_t blue16(Color16 c) noexcept {
	return (c & 0b0111110000000000) >> 10;
}

[[nodiscard]]
constexpr uint8_t alpha16(Color16 c) noexcept {
	return c >> 15;
}

[[nodiscard]]
constexpr uint8_t red32(Color16 c) noexcept {
	return red16(c) * 8;
}

[[nodiscard]]
constexpr uint8_t green32(Color16 c) noexcept {
	return green16(c) * 8;
}

[[nodiscard]]
constexpr uint8_t blue32(Color16 c) noexcept {
	return blue16(c) * 8;
}

[[nodiscard]]
constexpr uint8_t alpha32(Color16 c) noexcept {
	return (c >> 15) * 255;
}


[[nodiscard]]
constexpr uint8_t red32(Color32 c) noexcept {
	return (c & 0x000000ff) >> 0;
}

[[nodiscard]]
constexpr uint8_t green32(Color32 c) noexcept {
	return (c & 0x0000ff00) >> 8;
}

[[nodiscard]]
constexpr uint8_t blue32(Color32 c) noexcept {
	return (c & 0x00ff0000) >> 16;
}


[[nodiscard]]
constexpr Color16 color16(uint8_t r, uint8_t g, uint8_t b) {
	return r | (g << 5) | (b << 10);
}

static_assert(color16(0, 31, 0) == 992);
static_assert(color16(16, 31, 0) == 1008);
static_assert(color16(16, 31, 8) == 9200);

}
