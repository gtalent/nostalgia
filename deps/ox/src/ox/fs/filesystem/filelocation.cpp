/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "filelocation.hpp"

namespace ox {

FileLocation::~FileLocation() {
	if (m_type == Path && m_data.path) {
		delete m_data.path;
		m_data.path = nullptr;
	}
}

}
