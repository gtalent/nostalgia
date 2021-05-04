/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <json/json.h>

#include <ox/model/optype.hpp>
#include <ox/model/types.hpp>
#include <ox/std/buffer.hpp>
#include <ox/std/hashmap.hpp>
#include <ox/std/string.hpp>

namespace ox {

class OrganicClawWriter {

	template<typename T>
	friend Result<Buffer> writeOC(T *val);

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
		Error field(const char*, HashMap<String, T> *val);

		template<std::size_t L>
		Error field(const char*, BString<L> *val);

		Error field(const char*, String val);

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
		[[nodiscard]]
		constexpr bool targetValid() noexcept {
			return static_cast<int>(m_fieldIt) == m_unionIdx || m_unionIdx == -1;
		}

		[[nodiscard]]
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
Error OrganicClawWriter::field(const char *key, BString<L> *val) {
	return field(key, SerStr(val->data(), val->cap()));
}

template<typename T>
Error OrganicClawWriter::field(const char *key, T *val) {
	if constexpr(isVector_v<T>) {
		return field(key, val->data(), val->size());
	} else if (val && targetValid()) {
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
Error OrganicClawWriter::field(const char *key, HashMap<String, T> *val) {
	if (targetValid()) {
		const auto &keys = val->keys();
		OrganicClawWriter w;
		for (std::size_t i = 0; i < keys.size(); ++i) {
			const auto k = keys[i].c_str();
			oxReturnError(w.field(k, &val->operator[](k)));
		}
		value(key) = w.m_json;
	}
	++m_fieldIt;
	return OxError(0);
}

template<typename T>
Result<Buffer> writeOC(T *val) {
	OrganicClawWriter writer;
	oxReturnError(model(&writer, val));
	Json::StreamWriterBuilder jsonBuilder;
	const auto str = Json::writeString(jsonBuilder, writer.m_json);
	Buffer buff(str.size() + 1);
	memcpy(buff.data(), str.c_str(), str.size() + 1);
	return move(buff);
}

}
