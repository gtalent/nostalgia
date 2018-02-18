/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "types.hpp"

int ox_memcmp(const void *ptr1, const void *ptr2, size_t size);

void *ox_memcpy(void *dest, const void *src, int64_t size);

void *ox_memset(void *ptr, int val, int64_t size);
