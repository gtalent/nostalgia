/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <json/json.h>

#include <ox/model/optype.hpp>
#include <ox/model/types.hpp>
#include <ox/std/byteswap.hpp>
#include <ox/std/memops.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

namespace ox {

template<typename Key>
class OrganicClawReader {

	private:
		Json::Value m_json;
		int m_fields = 0;
		int m_field = 0;
		std::size_t m_buffIt = 0;
		std::size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;

	public:
		OrganicClawReader(Json::Value json = {});

		~OrganicClawReader();

		[[nodiscard]] Error field(Key key, int8_t *val);
		[[nodiscard]] Error field(Key key, int16_t *val);
		[[nodiscard]] Error field(Key key, int32_t *val);
		[[nodiscard]] Error field(Key key, int64_t *val);

		[[nodiscard]] Error field(Key key, uint8_t *val);
		[[nodiscard]] Error field(Key key, uint16_t *val);
		[[nodiscard]] Error field(Key key, uint32_t *val);
		[[nodiscard]] Error field(Key key, uint64_t *val);

		[[nodiscard]] Error field(Key key, bool *val);

		// array handler
		template<typename T>
		[[nodiscard]] Error field(Key key, T *val, std::size_t len);

		template<typename T>
		[[nodiscard]] Error field(Key key, ox::Vector<T> *val);

		template<typename T>
		[[nodiscard]] Error field(Key key, T *val);

		template<std::size_t L>
		[[nodiscard]] Error field(Key key, ox::BString<L> *val);

		[[nodiscard]] Error field(Key key, SerStr val);

		/**
		 * Reads an array length from the current location in the buffer.
		 * @param pass indicates that the parsing should iterate past the array length
		 */
		[[nodiscard]] std::size_t arrayLength(Key key, bool pass = true);

		/**
		 * Reads an string length from the current location in the buffer.
		 */
		[[nodiscard]] std::size_t stringLength(Key name);

		void setTypeInfo(const char *name, int fields);

		/**
		 * Returns a OrganicClawReader to parse a child object.
		 */
		[[nodiscard]] OrganicClawReader child(Key key);

		static constexpr OpType opType() {
			return OpType::Read;
		}

};

template<typename Key>
template<typename T>
Error OrganicClawReader<Key>::field(Key key, T *val) {
	if (val) {
		auto reader = child(key);
		oxReturnError(model(&reader, val));
	}
	return OxError(0);
}

template<typename Key>
template<std::size_t L>
Error OrganicClawReader<Key>::field(Key name, ox::BString<L> *val) {
	return field(name, SerStr(val->data(), val->cap()));
}

// array handler
template<typename Key>
template<typename T>
Error OrganicClawReader<Key>::field(Key key, T *val, std::size_t valLen) {
	const auto &srcVal = m_json[key];
	auto srcSize = srcVal.size();
	if (srcSize > valLen) {
		return OxError(1);
	}
	OrganicClawReader<Json::ArrayIndex> r(srcVal);
	for (decltype(srcSize) i = 0; i < srcSize; ++i) {
		oxReturnError(r.field(i, &val[i]));
	}
	return OxError(0);
}

template<typename Key>
template<typename T>
Error OrganicClawReader<Key>::field(Key key, ox::Vector<T> *val) {
	return field(nullptr, val->data(), val->size());
}

template<typename T>
ValErr<T> readOC(const char *json) {
	Json::Value doc;
	Json::CharReaderBuilder parserBuilder;
	auto parser = std::unique_ptr<Json::CharReader>(parserBuilder.newCharReader());
	if (!parser->parse(json, json + ox_strlen(json), &doc, nullptr)) {
		return OxError(1, "Could not parse JSON");
	}
	OrganicClawReader<const char*> reader(doc);
	T val;
	oxReturnError(model(&reader, &val));
	return {std::move(val), OxError(0)};
}

}
