/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/assert.hpp>
#include <ox/std/bit.hpp>
#include <ox/std/byteswap.hpp>
#include <ox/std/memops.hpp>

namespace ox::mc {

template<typename T>
static constexpr auto Bits = sizeof(T) << 3;

/**
 * Returns highest bit other than possible signed bit.
 * Bit numbering starts at 0.
 */
template<typename I>
[[nodiscard]] constexpr std::size_t highestBit(I val) noexcept {
	int shiftStart = sizeof(I) * 8 - 1;
	// find most significant non-sign indicator bit
	std::size_t highestBit = 0;
	// start at one bit lower if signed
	if constexpr(ox::is_signed_v<I>) {
		--shiftStart;
	}
	for (auto i = shiftStart; i > -1; --i) {
		const auto bitValue = (val >> i) & 1;
		if (bitValue) {
			highestBit = i;
			break;
		}
	}
	return highestBit;
}

static_assert(highestBit(int8_t(0b10000000)) == 0);
static_assert(highestBit(1) == 0);
static_assert(highestBit(2) == 1);
static_assert(highestBit(4) == 2);
static_assert(highestBit(8) == 3);
static_assert(highestBit(uint64_t(1) << 31) == 31);
static_assert(highestBit(uint64_t(1) << 63) == 63);

struct McInt {
	uint8_t data[9] = {0};
	// length of integer in bytes
	std::size_t length = 0;
};

template<typename I>
[[nodiscard]] constexpr McInt encodeInteger(I input) noexcept {
	McInt out;
	// move input to uint64_t to allow consistent bit manipulation, and to avoid
	// overflow concerns
	uint64_t val = 0;
	ox_memcpy(&val, &input, sizeof(I));
	if (val) {
		// bits needed to represent number factoring in space possibly
		// needed for signed bit
		const auto bits = highestBit(val) + 1 + (ox::is_signed_v<I> ? 1 : 0);
		// bytes needed to store value
		std::size_t bytes = bits / 8 + (bits % 8 != 0);
		const auto bitsAvailable = bytes * 8; // bits available to integer value
		const auto bitsNeeded = bits + bytes;
		// factor in bits needed for bytesIndicator (does not affect bytesIndicator)
		// bits for integer + bits neded to represent bytes > bits available
		if (bitsNeeded > bitsAvailable && bytes != 9) {
			++bytes;
		}
		const auto bytesIndicator = onMask<uint8_t>(bytes - 1);

		// ensure we are copying from little endian represenstation
		LittleEndian<I> leVal = val;
		if (bytes == 9) {
			out.data[0] = bytesIndicator;
			ox_memcpy(&out.data[1], &leVal, sizeof(I));
		} else {
			auto intermediate =
				static_cast<uint64_t>(leVal.raw()) << bytes |
				static_cast<uint64_t>(bytesIndicator);
			ox_memcpy(out.data, &intermediate, sizeof(intermediate));
		}
		out.length = bytes;
	}
	return out;
}

/**
 * Returns the number of bytes indicated by the bytes indicator of a variable
 * length integer.
 */
[[nodiscard]] static constexpr std::size_t countBytes(uint8_t b) noexcept {
	std::size_t i = 0;
	for (; (b >> i) & 1; i++);
	return i + 1;
}

static_assert(countBytes(0b00000000) == 1);
static_assert(countBytes(0b00000001) == 2);
static_assert(countBytes(0b00000011) == 3);
static_assert(countBytes(0b00000111) == 4);
static_assert(countBytes(0b00001111) == 5);
static_assert(countBytes(0b00011111) == 6);
static_assert(countBytes(0b00111111) == 7);
static_assert(countBytes(0b01111111) == 8);
static_assert(countBytes(0b11111111) == 9);

template<typename I>
Result<I> decodeInteger(uint8_t buff[9], std::size_t buffLen, std::size_t *bytesRead) noexcept {
	const auto bytes = countBytes(buff[0]);
	if (bytes == 9) {
		*bytesRead = bytes;
		I out = 0;
		memcpy(&out, &buff[1], sizeof(I));
		return static_cast<I>(LittleEndian<I>(out));
	} else if (buffLen >= bytes) {
		*bytesRead = bytes;
		uint64_t decoded = 0;
		memcpy(&decoded, &buff[0], bytes);
		decoded >>= bytes;
		auto out = static_cast<I>(decoded);
		// move sign bit
		if constexpr(ox::is_signed_v<I>) {
			const auto valBits = bytes << 3;
			// get sign
			uint64_t sign = decoded >> (valBits - 1);
			// remove sign
			decoded &= uint64_t(~0) ^ (uint64_t(1) << valBits);
			// set sign
			decoded |= sign << (Bits<I> - 1);
			memcpy(&out, &decoded, sizeof(out));
		}
		return out;
	}
	return OxError(1);
}

template<typename I>
Result<I> decodeInteger(McInt m) noexcept {
	std::size_t bytesRead;
	return decodeInteger<I>(m.data, 9, &bytesRead);
}

}
