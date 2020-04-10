/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <json/json.h>

#include <ox/model/optype.hpp>
#include <ox/model/types.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

namespace ox {

template<typename Key>
class OrganicClawWriter {
	friend OrganicClawWriter<const char*>;
	friend OrganicClawWriter<Json::ArrayIndex>;
	template<typename T>
	friend ValErr<String> writeOC(T *val);

	protected:
		Json::Value m_json;

	public:
		OrganicClawWriter() = default;

		OrganicClawWriter(Json::Value json);

		Error field(Key, int8_t *val);
		Error field(Key, int16_t *val);
		Error field(Key, int32_t *val);
		Error field(Key, int64_t *val);

		Error field(Key, uint8_t *val);
		Error field(Key, uint16_t *val);
		Error field(Key, uint32_t *val);
		Error field(Key, uint64_t *val);

		Error field(Key, bool *val);

		template<typename T>
		Error field(Key, T *val, std::size_t len);

		template<typename T>
		Error field(Key, ox::Vector<T> *val);

		template<std::size_t L>
		Error field(Key, ox::BString<L> *val);

		Error field(Key, ox::String val);

		Error field(Key, SerStr val);

		template<typename T>
		Error field(Key, T *val);

		void setTypeInfo(const char *name, int fields);

		static constexpr auto opType() {
			return OpType::Write;
		}

};

template<typename Key>
template<typename T>
Error OrganicClawWriter<Key>::field(Key key, T *val, std::size_t len) {
	OrganicClawWriter<Json::ArrayIndex> w;
	for (std::size_t i = 0; i < len; ++i) {
		oxReturnError(w.field(i, &val[i]));
	}
	m_json[key] = w.m_json;
	return OxError(0);
}

template<typename Key>
template<std::size_t L>
Error OrganicClawWriter<Key>::field(Key key, ox::BString<L> *val) {
	return field(key, SerStr(val->data(), val->cap()));
}

template<typename Key>
template<typename T>
Error OrganicClawWriter<Key>::field(Key key, T *val) {
	OrganicClawWriter<const char*> w;
	oxReturnError(model(&w, val));
	if (!w.m_json.isNull()) {
		m_json[key] = w.m_json;
	}
	return OxError(0);
}

template<typename Key>
template<typename T>
Error OrganicClawWriter<Key>::field(Key key, ox::Vector<T> *val) {
	return field(key, val->data(), val->size());
}


template<typename T>
ValErr<String> writeOC(T *val) {
	OrganicClawWriter<const char*> writer;
	oxReturnError(model(&writer, val));
	Json::StreamWriterBuilder jsonBuilder;
	return String(Json::writeString(jsonBuilder, writer.m_json).c_str());
}

extern template class OrganicClawWriter<const char*>;
extern template class OrganicClawWriter<Json::ArrayIndex>;

}
