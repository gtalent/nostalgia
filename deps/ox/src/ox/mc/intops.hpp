/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/assert.hpp>
#include <ox/std/byteswap.hpp>

namespace ox::mc {

/**
 * Returns highest bit other than possible signed bit.
 * Bit numbering starts at 0.
 */
template<typename I>
[[nodiscard]] constexpr auto highestBit(I val) noexcept {
	constexpr auto Bits = sizeof(I) * 8;
	// find most significant non-sign indicator bit
	std::size_t highestBit = Bits;
	// start at one bit lower if signed
	if constexpr(ox::is_signed<I>) {
		--highestBit;
	}
	for (int i = Bits - 1; i > -1; --i) {
		const auto bitValue = (val >> i) & 1;
		if (bitValue) {
			highestBit = i;
			break;
		}
	}
	return highestBit;
}

static_assert(highestBit(1) == 0);
static_assert(highestBit(2) == 1);
static_assert(highestBit(4) == 2);
static_assert(highestBit(8) == 3);
static_assert(highestBit(1 << 31) == 31);
static_assert(highestBit(uint64_t(1) << 63) == 63);

struct McInt {
	uint8_t data[9];
	// length of integer in bytes
	std::size_t length = 0;
};

template<typename I>
McInt encodeInteger(I val) noexcept {
	McInt out;
	if (val) {
		// bits needed to represent number factoring in space possibly
		// needed for signed bit
		const auto bits = highestBit(val) + 1 + (ox::is_signed<I> ? 1 : 0);
		// bytes needed to store value
		std::size_t bytes = bits / 8 + (bits % 8 != 0);
		auto bitsAvailable = bytes * 8;
		// factor in bits needed for bytesIndicator (does not affect bytesIndicator)
		// bits for integer + bits neded to represent bytes > bits available
		if (bits + bytes > bitsAvailable && bytes != 9) {
			++bytes;
			bitsAvailable = bytes * 8;
		}
		const auto bytesIndicator = onMask<uint8_t>(bytes - 1); // << (7 - bytes);

		// move sign bit
		if constexpr(ox::is_signed<I>) {
			if (val < 0) {
				val ^= int64_t(1) << (sizeof(I) * 8 - 1);
				val |= int64_t(1) << (bitsAvailable - 1);
			}
		}
		// ensure we are copying from little endian represenstation
		LittleEndian<I> leVal = val;
		if (bytes == 9) {
			out.data[0] = bytesIndicator;
			*reinterpret_cast<I*>(&out.data[1]) = *reinterpret_cast<I*>(&leVal);
		} else {
			*reinterpret_cast<I*>(&out.data[0]) = (leVal << bytes) | bytesIndicator;
		}
		out.length = bytes;
	}
	return out;
}

}
