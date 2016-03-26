/*
 * Copyright 2016 gtalent2@gmail.com
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef NOSTALGIA_COMMON_MEMOPS_HPP
#define NOSTALGIA_COMMON_MEMOPS_HPP

namespace nostalgia {
namespace common {

void memcpy(void *src, void *dest, int size);

void memset(void *ptr, char val, int size);

}
}

#endif
