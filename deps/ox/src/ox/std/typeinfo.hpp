/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#if __has_include(<typeinfo>)
#include <typeinfo>
#else

namespace std {

// this is not at all guaranteed to work, and does not even fully comply with
// what little the standard does define
struct type_info {
	private:
		const char *m_name = "";

	protected:
		explicit type_info(const char *name): m_name(name) {
		}

		const char *name() {
			return m_name;
		}
};

}

#endif
