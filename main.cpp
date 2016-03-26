/*
 * Copyright 2016 gtalent2@gmail.com
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

int main() {
	*(unsigned int*)0x04000000 = 0x0403;

	((unsigned short*)0x06000000)[120+80*240] = 0x001F;
	((unsigned short*)0x06000000)[136+80*240] = 0x03E0;
	((unsigned short*)0x06000000)[120+96*240] = 0x7C00;

	while(1);

	return 0;
}
