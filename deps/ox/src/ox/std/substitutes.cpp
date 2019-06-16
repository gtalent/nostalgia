/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef OX_USE_STDLIB

// weak link panic to ensure that it exists, but allow it to be overriden by
// platform specific code
void __attribute__((weak)) panic(const char*) {
	while (1);
}

extern "C" {
	
void __cxa_pure_virtual() { 
	panic("Missing virtual function");
}

}

#endif
