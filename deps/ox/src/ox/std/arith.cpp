/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "hardware.hpp"

// these provide software implementations of arithmetic operators expected by
// the compiler on older processors

#if !OX_HW_DIV

extern "C" {

unsigned __aeabi_uldivmod(unsigned, unsigned) {
	return 0;
}

unsigned __aeabi_uidiv(unsigned, unsigned) {
	return 0;
}

unsigned __aeabi_uimod(unsigned, unsigned) {
	return 0;
}

unsigned __aeabi_uidivmod(unsigned, unsigned) {
	return 0;
}

signed __aeabi_idiv(signed, signed) {
	return 0;
}

signed __aeabi_imod(signed, signed) {
	return 0;
}

}

#endif
