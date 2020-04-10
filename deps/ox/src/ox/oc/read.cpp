/*
 * Copyright 2015 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/bit.hpp>

#include "read.hpp"

namespace ox {

template<typename Key>
OrganicClawReader<Key>::OrganicClawReader(const uint8_t *buff, std::size_t buffSize) {
	auto json = bit_cast<const char*>(buff);
	auto jsonLen = ox_strnlen(json, buffSize);
	Json::CharReaderBuilder parserBuilder;
	auto parser = std::unique_ptr<Json::CharReader>(parserBuilder.newCharReader());
	if (!parser->parse(json, json + jsonLen, &m_json, nullptr)) {
		throw OxError(1, "Could not parse JSON");
	}
}

template<typename Key>
OrganicClawReader<Key>::OrganicClawReader(const char *json, std::size_t jsonLen) {
	Json::CharReaderBuilder parserBuilder;
	auto parser = std::unique_ptr<Json::CharReader>(parserBuilder.newCharReader());
	if (!parser->parse(json, json + jsonLen, &m_json, nullptr)) {
		throw OxError(1, "Could not parse JSON");
	}
}

template<typename Key>
OrganicClawReader<Key>::OrganicClawReader(const Json::Value &json) {
	m_json = json;
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, int8_t *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isInt()) {
		*val = static_cast<int8_t>(jv.asInt());
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, int16_t *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isInt()) {
		*val = static_cast<int16_t>(jv.asInt());
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, int32_t *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isInt()) {
		*val = static_cast<int32_t>(jv.asInt());
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, int64_t *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isInt() || jv.isInt64()) {
		*val = static_cast<int64_t>(jv.asInt64());
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}


template<typename Key>
Error OrganicClawReader<Key>::field(Key key, uint8_t *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isUInt()) {
		*val = static_cast<uint8_t>(jv.asUInt());
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, uint16_t *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isUInt()) {
		*val = static_cast<uint16_t>(jv.asUInt());
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, uint32_t *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isUInt()) {
		*val = static_cast<uint32_t>(jv.asUInt());
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, uint64_t *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isUInt() || jv.isUInt64()) {
		*val = static_cast<uint64_t>(jv.asUInt64());
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, bool *val) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isBool()) {
		*val = jv.asBool();
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, SerStr val) {
	const char *begin = nullptr, *end = nullptr;
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return OxError(0);
	}
	if (jv.isString()) {
		jv.getString(&begin, &end);
		auto strSize = end - begin;
		if (strSize >= val.cap()) {
			return OxError(1, "String size exceeds capacity of destination");
		}
		ox_memcpy(val.data(), begin, static_cast<std::size_t>(strSize));
		val.data()[strSize] = 0;
		return OxError(0);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
[[nodiscard]] ValErr<std::size_t> OrganicClawReader<Key>::arrayLength(Key key, bool) {
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return 0;
	}
	if (jv.isArray()) {
		return jv.size();
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
[[nodiscard]] std::size_t OrganicClawReader<Key>::stringLength(Key key) {
	const char *begin = nullptr, *end = nullptr;
	const auto &jv = value(key);
	++m_fieldIt;
	if (jv.empty()) {
		return 0;
	}
	if (jv.isString()) {
		jv.getString(&begin, &end);
		return static_cast<std::size_t>(end - begin);
	}
	return OxError(1, "Type mismatch");
}

template<typename Key>
void OrganicClawReader<Key>::setTypeInfo(const char*, int) {
}

template<typename Key>
OrganicClawReader<Key> OrganicClawReader<Key>::child(Key key) {
	return OrganicClawReader<Key>(m_json[key]);
}

template<typename Key>
bool OrganicClawReader<Key>::fieldPresent(Key key) {
	return !m_json[key].empty();
}

template<typename Key>
Json::Value &OrganicClawReader<Key>::value(Key key) {
	if (m_json.isArray()) {
		return m_json[m_fieldIt];
	} else {
		return m_json[key];
	}
}

template class OrganicClawReader<const char*>;
template class OrganicClawReader<Json::ArrayIndex>;

}
