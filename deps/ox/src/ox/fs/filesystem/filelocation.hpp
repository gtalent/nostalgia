/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/std.hpp>

namespace ox {

class FileLocation {
	private:
		enum {
			None  = -1,
			Path  = 0,
			Inode = 1,
		} m_type = None;
		union {
			char *path;
			uint64_t inode;
		} m_data;

	public:
		~FileLocation();

};

}
