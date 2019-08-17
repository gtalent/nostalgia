/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "json_read.hpp"
#include "json_write.hpp"

namespace nostalgia::studio {

class JsonOperator {

	public:
		virtual ~JsonOperator() = default;

		virtual int op(QString fieldName, int *dest) = 0;

		virtual int op(QString fieldName, bool *dest) = 0;

		virtual int op(QString fieldName, double *dest) = 0;

		virtual int op(QString fieldName, QString *dest) = 0;

};

}
