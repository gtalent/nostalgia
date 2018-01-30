/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>

uint8_t *loadFileBuff(FILE *file, size_t *sizeOut = nullptr);

uint8_t *loadFileBuff(const char *path, size_t *sizeOut = nullptr);
