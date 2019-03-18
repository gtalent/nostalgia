/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
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

namespace nostalgia {
namespace studio {

class JsonReader {

	private:
		QJsonObject &m_src;

	public:
		JsonReader(QJsonObject &obj);

		ox::Error setTypeInfo(const char*, int) { return OxError(0); };

		ox::Error field(QString fieldName, int *dest);

		ox::Error field(QString fieldName, bool *dest);

		ox::Error field(QString fieldName, double *dest);

		ox::Error field(QString fieldName, QString *dest);

		template<typename T>
		ox::Error field(QString fieldName, T *dest);

		template<typename T>
		ox::Error field(QString fieldName, QVector<T> *dest);

	private:
		ox::Error field(QJsonValueRef src, int *dest);

		ox::Error field(QJsonValueRef src, bool *dest);

		ox::Error field(QJsonValueRef src, double *dest);

		ox::Error field(QJsonValueRef src, QString *dest);

		template<typename T>
		ox::Error field(QJsonValueRef src, T *dest);

};

template<typename T>
ox::Error JsonReader::field(QString fieldName, T *dest) {
	if (m_src.contains(fieldName)) {
		auto obj = m_src[fieldName].toObject();
		auto reader = JsonReader(obj);
		return model(&reader, dest);
	} else {
		return JSON_ERR_FIELD_MISSING;
	}
};

template<typename T>
ox::Error JsonReader::field(QString fieldName, QVector<T> *dest) {
	ox::Error err = 0;
	if (m_src.contains(fieldName)) {
		auto a = m_src[fieldName].toArray();
		dest->resize(a.size());
		for (int i = 0; i < dest->size(); i++) {
			err |= field(a[i], &(*dest)[i]);
		}
	} else {
		err |= JSON_ERR_FIELD_MISSING;
	}
	return err;
};

template<typename T>
ox::Error JsonReader::field(QJsonValueRef src, T *dest) {
	auto obj = src.toObject();
	auto reader = JsonReader(obj);
	return model(&reader, dest);
}

template<typename T>
ox::Error readJson(QString json, T *dest) {
	auto obj = QJsonDocument::fromJson(json.toUtf8()).object();
	JsonReader rdr(obj);
	return model(&rdr, dest);
}

}
}
