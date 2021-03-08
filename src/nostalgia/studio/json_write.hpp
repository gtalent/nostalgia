/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <ox/std/error.hpp>

#include "json_err.hpp"

namespace nostalgia::studio {

class JsonWriter {

	private:
		QJsonObject &m_dest;

	public:
		explicit JsonWriter(QJsonObject &obj);

		ox::Error setTypeInfo(const char*, int) { return OxError(0); };

		ox::Error field(QString fieldName, int *src);

		ox::Error field(QString fieldName, bool *src);

		ox::Error field(QString fieldName, double *src);

		ox::Error field(QString fieldName, QString *src);

		template<typename T>
		ox::Error field(QString fieldName, T *src);

		template<typename T>
		ox::Error field(QString fieldName, QVector<T> *src);

};

template<typename T>
ox::Error JsonWriter::field(QString fieldName, T *src) {
	auto obj = QJsonObject();
	auto reader = JsonWriter(obj);
	auto err = model(&reader, src);
	m_dest[fieldName] = obj;
	return err;
}

template<typename T>
ox::Error JsonWriter::field(QString fieldName, QVector<T> *src) {
	auto err = OxError(0);
	QJsonArray a;
	for (int i = 0; i < src->size(); i++) {
		err |= field(a[i], &src->at(i));
	}
	m_dest[fieldName] = a;
	return err;
}

template<typename T>
ox::Error writeJson(QString *json, T *src) {
	auto obj = QJsonObject();
	JsonWriter rdr(obj);
	auto err = model(&rdr, src);
	*json = QJsonDocument(obj).toJson();
	return err;
}

}
