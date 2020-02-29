/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "json_read.hpp"

namespace nostalgia::studio {

JsonReader::JsonReader(QJsonObject &obj): m_src(obj) {
}

ox::Error JsonReader::field(QString fieldName, int *dest) {
	if (m_src.contains(fieldName)) {
		return field(m_src[fieldName], dest);
	} else {
		return OxError(JSON_ERR_FIELD_MISSING);
	}
}

ox::Error JsonReader::field(QString fieldName, bool *dest) {
	if (m_src.contains(fieldName)) {
		return field(m_src[fieldName], dest);
	} else {
		return OxError(JSON_ERR_FIELD_MISSING);
	}
}

ox::Error JsonReader::field(QString fieldName, double *dest) {
	if (m_src.contains(fieldName)) {
		return field(m_src[fieldName], dest);
	} else {
		return OxError(JSON_ERR_FIELD_MISSING);
	}
}

ox::Error JsonReader::field(QString fieldName, QString *dest) {
	if (m_src.contains(fieldName)) {
		return field(m_src[fieldName], dest);
	} else {
		return OxError(JSON_ERR_FIELD_MISSING);
	}
}



ox::Error JsonReader::field(QJsonValueRef src, int *dest) {
	if (src.isDouble()) {
		*dest = src.toInt();
		return OxError(0);
	} else {
		return OxError(JSON_ERR_UNEXPECTED_TYPE);
	}
}

ox::Error JsonReader::field(QJsonValueRef src, bool *dest) {
	if (src.isBool()) {
		*dest = src.toBool();
		return OxError(0);
	} else {
		return OxError(JSON_ERR_UNEXPECTED_TYPE);
	}
}

ox::Error JsonReader::field(QJsonValueRef src, double *dest) {
	if (src.isDouble()) {
		*dest = src.toDouble();
		return OxError(0);
	} else {
		return OxError(JSON_ERR_UNEXPECTED_TYPE);
	}
}

ox::Error JsonReader::field(QJsonValueRef src, QString *dest) {
	if (src.isString()) {
		*dest = src.toString();
		return OxError(0);
	} else {
		return OxError(JSON_ERR_UNEXPECTED_TYPE);
	}
}


}
