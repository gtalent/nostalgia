/*
 * Copyright 2016 gtalent2@gmail.com
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef NOSTALGIA_CORE_TYPES_HPP
#define NOSTALGIA_CORE_TYPES_HPP

#include <cstdint>

namespace nostalgia {
namespace core {

typedef char               int8_t;
typedef unsigned char      uint8_t;
typedef short              int16_t;
typedef unsigned short     uint16_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
typedef unsigned           uint_t;
typedef long long          int64_t;
typedef unsigned long long uint64_t;

#ifdef _LP64
typedef uint64_t size_t;
#elif _LP32
typedef uint32_t size_t;
#endif

}
}

#endif
