/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "json_err.hpp"
#include "json_operator.hpp"

namespace nostalgia {
namespace studio {

class JsonWriter {

	private:
		QJsonObject &m_dest;

	public:
		JsonWriter(QJsonObject &obj);

		ox::Error op(QString fieldName, int *src);

		ox::Error op(QString fieldName, bool *src);

		ox::Error op(QString fieldName, double *src);

		ox::Error op(QString fieldName, QString *src);

		template<typename T>
		ox::Error op(QString fieldName, T *src);

		template<typename T>
		ox::Error op(QString fieldName, QVector<T> *src);

};

template<typename T>
ox::Error JsonWriter::op(QString fieldName, T *src) {
	auto obj = QJsonObject();
	auto reader = JsonWriter(obj);
	auto err = ioOp(&reader, src);
	m_dest[fieldName] = obj;
	return err;
};

template<typename T>
ox::Error JsonWriter::op(QString fieldName, QVector<T> *src) {
	ox::Error err = 0;
	auto &a = m_dest[fieldName] = QJsonArray();
	for (int i = 0; i < src->size(); i++) {
		err |= op(a[i], &src->at(i));
	}
	return err;
};

template<typename T>
int writeJson(QString *json, T *src) {
	auto obj = QJsonObject();
	JsonWriter rdr(obj);
	auto err = ioOp(&rdr, src);
	*json = QJsonDocument(obj).toJson();
	return err;
}

}
}
