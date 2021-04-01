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
#include <ox/std/byteswap.hpp>
#include <ox/std/hashmap.hpp>
#include <ox/std/memops.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

namespace ox {

class OrganicClawReader {

	private:
		Json::Value m_json;
		Json::ArrayIndex m_fieldIt = 0;
		int m_unionIdx = -1;

	public:
		OrganicClawReader() = default;

		OrganicClawReader(const uint8_t *buff, std::size_t buffSize);

		OrganicClawReader(const char *json, std::size_t buffSize);

		OrganicClawReader(const Json::Value &json, int unionIdx = -1);

		Error field(const char *key, int8_t *val);
		Error field(const char *key, int16_t *val);
		Error field(const char *key, int32_t *val);
		Error field(const char *key, int64_t *val);

		Error field(const char *key, uint8_t *val);
		Error field(const char *key, uint16_t *val);
		Error field(const char *key, uint32_t *val);
		Error field(const char *key, uint64_t *val);

		Error field(const char *key, bool *val);

		// array handler
		template<typename T>
		Error field(const char *key, T *val, std::size_t len);

		template<typename T>
		Error field(const char *key, Vector<T> *val);

		template<typename T>
		Error field(const char*, HashMap<String, T> *val);

		template<typename T>
		Error field(const char *key, T *val);

		template<typename U>
		Error field(const char *key, UnionView<U> val);

		template<std::size_t L>
		Error field(const char *key, ox::BString<L> *val);

		Error field(const char *key, SerStr val);

		/**
		 * Reads an array length from the current location in the buffer.
		 * @param pass indicates that the parsing should iterate past the array length
		 */
		Result<std::size_t> arrayLength(const char *key, bool pass = true);

		/**
		 * Reads an string length from the current location in the buffer.
		 */
		[[nodiscard]] std::size_t stringLength(const char *name);

		template<typename T = void>
		constexpr void setTypeInfo(const char* = T::TypeName, int = T::Fields) {
		}

		/**
		 * Returns a OrganicClawReader to parse a child object.
		 */
		[[nodiscard]] OrganicClawReader child(const char *key, int unionIdx = -1);

		// compatibility stub
		constexpr void nextField() noexcept {}

		bool fieldPresent(const char *key);

		static constexpr auto opType() {
			return OpType::Read;
		}

	private:

		Json::Value &value(const char *key);

		bool targetValid() noexcept;

};

template<typename T>
Error OrganicClawReader::field(const char *key, T *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty() || jv.isObject()) {
			auto reader = child(key);
			return model(&reader, val);
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

template<typename U>
Error OrganicClawReader::field(const char *key, UnionView<U> val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty() || jv.isObject()) {
			auto reader = child(key, val.idx());
			err = model(&reader, val.get());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

template<std::size_t L>
Error OrganicClawReader::field(const char *name, ox::BString<L> *val) {
	return field(name, SerStr(val->data(), val->cap()));
}

// array handler
template<typename T>
Error OrganicClawReader::field(const char *key, T *val, std::size_t valLen) {
	const auto &srcVal = value(key);
	auto srcSize = srcVal.size();
	if (srcSize > valLen) {
		return OxError(1);
	}
	OrganicClawReader r(srcVal);
	for (decltype(srcSize) i = 0; i < srcSize; ++i) {
		oxReturnError(r.field("", &val[i]));
	}
	return OxError(0);
}

template<typename T>
Error OrganicClawReader::field(const char *key, ox::Vector<T> *val) {
	return field(key, val->data(), val->size());
}

template<typename T>
Error OrganicClawReader::field(const char *key, HashMap<String, T> *val) {
	const auto &srcVal = value(key);
	auto keys = srcVal.getMemberNames();
	auto srcSize = srcVal.size();
	OrganicClawReader r(srcVal);
	for (decltype(srcSize) i = 0; i < srcSize; ++i) {
		auto k = keys[i].c_str();
		oxReturnError(r.field(k, &val->operator[](k)));
	}
	return OxError(0);
}

template<typename T>
Error readOC(const char *json, std::size_t jsonSize, T *val) noexcept {
	// OrganicClawReader constructor can throw, but readOC should return its errors.
	try {
		Json::Value doc;
		Json::CharReaderBuilder parserBuilder;
		auto parser = std::unique_ptr<Json::CharReader>(parserBuilder.newCharReader());
		if (!parser->parse(json, json + jsonSize, &doc, nullptr)) {
			return OxError(1, "Could not parse JSON");
		}
		OrganicClawReader reader(json, jsonSize);
		return model(&reader, val);
	} catch (Error err) {
		return err;
	} catch (...) {
		return OxError(1, "Unkown Error");
	}
}

template<typename T>
Result<std::unique_ptr<T>> readOC(const char *json) {
	auto val = std::make_unique<T>();
	oxReturnError(readOC(json, ox_strlen(json), val.get()));
	return {std::move(val), OxError(0)};
}

}
