/*
 * Copyright 2016 gtalent2@gmail.com
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "memops.hpp"

namespace nostalgia {
namespace common {

void memcpy(void *src, void *dest, int size) {
	char *srcBuf = (char*) src;
	char *dstBuf = (char*) dest;
	for (int i = 0; i < size; i++) {
		dstBuf[i] = (char) srcBuf[i];
	}
}

void memset(void *ptr, char val, int size) {
	char *buf = (char*) ptr;
	for (int i = 0; i < size; i++) {
		buf[i] = val;
	}
}

}
}
