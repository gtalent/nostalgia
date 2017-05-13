/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "json_write.hpp"

namespace nostalgia {
namespace studio {

JsonWriter::JsonWriter(QJsonObject &obj): m_dest(obj) {
}

ox::Error JsonWriter::op(QString fieldName, int *src) {
	m_dest[fieldName] = *src;
	return 0;
}

ox::Error JsonWriter::op(QString fieldName, bool *src) {
	m_dest[fieldName] = *src;
	return 0;
}

ox::Error JsonWriter::op(QString fieldName, double *src) {
	m_dest[fieldName] = *src;
	return 0;
}

ox::Error JsonWriter::op(QString fieldName, QString *src) {
	m_dest[fieldName] = *src;
	return 0;
}

}
}
