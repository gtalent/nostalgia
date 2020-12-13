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
#include <ox/std/hashmap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

namespace ox {

class OrganicClawWriter {

	template<typename T>
	friend Result<Vector<char>> writeOC(T *val);

	protected:
		Json::Value m_json;
		Json::ArrayIndex m_fieldIt = 0;
		int m_unionIdx = -1;

	public:
		OrganicClawWriter(int unionIdx = -1);

		OrganicClawWriter(Json::Value json, int unionIdx = -1);

		Error field(const char*, int8_t *val);
		Error field(const char*, int16_t *val);
		Error field(const char*, int32_t *val);
		Error field(const char*, int64_t *val);

		Error field(const char*, uint8_t *val);
		Error field(const char*, uint16_t *val);
		Error field(const char*, uint32_t *val);
		Error field(const char*, uint64_t *val);

		Error field(const char*, bool *val);

		template<typename T>
		Error field(const char*, T *val, std::size_t len);

		template<typename U>
		Error field(const char*, UnionView<U> val);

		template<typename T>
		Error field(const char*, ox::Vector<T> *val);

		template<typename T>
		Error field(const char*, HashMap<String, T> *val);

		template<std::size_t L>
		Error field(const char*, ox::BString<L> *val);

		Error field(const char*, ox::String val);

		Error field(const char*, SerStr val);

		template<typename T>
		Error field(const char*, T *val);

		template<typename T = void>
		constexpr void setTypeInfo(const char* = T::TypeName, int = T::Fields) {
		}

		static constexpr auto opType() {
			return OpType::Write;
		}

	private:
		constexpr bool targetValid() noexcept {
			return static_cast<int>(m_fieldIt) == m_unionIdx || m_unionIdx == -1;
		}

		Json::Value &value(const char *key);

};

template<typename T>
Error OrganicClawWriter::field(const char *key, T *val, std::size_t len) {
	if (targetValid()) {
		OrganicClawWriter w((Json::Value(Json::arrayValue)));
		for (std::size_t i = 0; i < len; ++i) {
			oxReturnError(w.field("", &val[i]));
		}
		value(key) = w.m_json;
	}
	++m_fieldIt;
	return OxError(0);
}

template<std::size_t L>
Error OrganicClawWriter::field(const char *key, ox::BString<L> *val) {
	return field(key, SerStr(val->data(), val->cap()));
}

template<typename T>
Error OrganicClawWriter::field(const char *key, T *val) {
	if (val && targetValid()) {
		OrganicClawWriter w;
		oxReturnError(model(&w, val));
		if (!w.m_json.isNull()) {
			value(key) = w.m_json;
		}
	}
	++m_fieldIt;
	return OxError(0);
}

template<typename U>
Error OrganicClawWriter::field(const char *key, UnionView<U> val) {
	if (targetValid()) {
		OrganicClawWriter w(val.idx());
		oxReturnError(model(&w, val.get()));
		if (!w.m_json.isNull()) {
			value(key) = w.m_json;
		}
	}
	++m_fieldIt;
	return OxError(0);
}

template<typename T>
Error OrganicClawWriter::field(const char *key, ox::Vector<T> *val) {
	return field(key, val->data(), val->size());
}

template<typename T>
Error OrganicClawWriter::field(const char *key, ox::HashMap<String, T> *val) {
	if (targetValid()) {
		auto &keys = val->keys();
		OrganicClawWriter w;
		for (std::size_t i = 0; i < keys.size(); ++i) {
			auto k = keys[i].c_str();
			oxReturnError(w.field(k, &val->at(k)));
		}
		value(key) = w.m_json;
	}
	++m_fieldIt;
	return OxError(0);
}

template<typename T>
Result<Vector<char>> writeOC(T *val) {
	OrganicClawWriter writer;
	oxReturnError(model(&writer, val));
	Json::StreamWriterBuilder jsonBuilder;
	auto str = Json::writeString(jsonBuilder, writer.m_json);
	Vector<char> buff(str.size() + 1);
	memcpy(buff.data(), str.c_str(), str.size() + 1);
	return buff;
}

}
