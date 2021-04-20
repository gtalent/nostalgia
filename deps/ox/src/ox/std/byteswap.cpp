/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "byteswap.hpp"

namespace ox {

template<typename T>
static constexpr bool testLittleEndian(T i) noexcept {
	return LittleEndian<T>(i) == i;
}

template<typename T>
static constexpr bool testBigEndian(T i) noexcept {
	return BigEndian<T>(i) == i;
}

static_assert(testLittleEndian<uint16_t>(0x00ff), "Test LittleEndian 0x00ff");
static_assert(testLittleEndian<uint16_t>(0xff00), "Test LittleEndian 0xff00");

static_assert(testLittleEndian<uint32_t>(0x000000ff), "Test LittleEndian 0x000000ff");
static_assert(testLittleEndian<uint32_t>(0x0000ff00), "Test LittleEndian 0x0000ff00");
static_assert(testLittleEndian<uint32_t>(0x00ff0000), "Test LittleEndian 0x00ff0000");
static_assert(testLittleEndian<uint32_t>(0xff000000), "Test LittleEndian 0xff000000");

static_assert(testLittleEndian<uint64_t>(0x00000000000000ff), "Test LittleEndian 0x00000000000000ff");
static_assert(testLittleEndian<uint64_t>(0x000000000000ff00), "Test LittleEndian 0x000000000000ff00");
static_assert(testLittleEndian<uint64_t>(0x0000000000ff0000), "Test LittleEndian 0x0000000000ff0000");
static_assert(testLittleEndian<uint64_t>(0x00000000ff000000), "Test LittleEndian 0x00000000ff000000");
static_assert(testLittleEndian<uint64_t>(0x000000ff00000000), "Test LittleEndian 0x000000ff00000000");
static_assert(testLittleEndian<uint64_t>(0x0000ff0000000000), "Test LittleEndian 0x0000ff0000000000");
static_assert(testLittleEndian<uint64_t>(0x00ff000000000000), "Test LittleEndian 0x00ff000000000000");
static_assert(testLittleEndian<uint64_t>(0xff00000000000000), "Test LittleEndian 0xff00000000000000");


static_assert(testBigEndian<uint16_t>(0x00ff), "Test BigEndian 0x00ff");
static_assert(testBigEndian<uint16_t>(0xff00), "Test BigEndian 0xff00");

static_assert(testBigEndian<uint32_t>(0x000000ff), "Test BigEndian 0x000000ff");
static_assert(testBigEndian<uint32_t>(0x0000ff00), "Test BigEndian 0x0000ff00");
static_assert(testBigEndian<uint32_t>(0x00ff0000), "Test BigEndian 0x00ff0000");
static_assert(testBigEndian<uint32_t>(0xff000000), "Test BigEndian 0xff000000");

static_assert(testBigEndian<uint64_t>(0x00000000000000ff), "Test BigEndian 0x00000000000000ff");
static_assert(testBigEndian<uint64_t>(0x000000000000ff00), "Test BigEndian 0x000000000000ff00");
static_assert(testBigEndian<uint64_t>(0x0000000000ff0000), "Test BigEndian 0x0000000000ff0000");
static_assert(testBigEndian<uint64_t>(0x00000000ff000000), "Test BigEndian 0x00000000ff000000");
static_assert(testBigEndian<uint64_t>(0x000000ff00000000), "Test BigEndian 0x000000ff00000000");
static_assert(testBigEndian<uint64_t>(0x0000ff0000000000), "Test BigEndian 0x0000ff0000000000");
static_assert(testBigEndian<uint64_t>(0x00ff000000000000), "Test BigEndian 0x00ff000000000000");
static_assert(testBigEndian<uint64_t>(0xff00000000000000), "Test BigEndian 0xff00000000000000");

}
