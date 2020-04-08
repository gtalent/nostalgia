/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "read.hpp"

namespace ox {

template<typename Key>
OrganicClawReader<Key>::OrganicClawReader(Json::Value json) {
	m_json = json;
}

template<typename Key>
OrganicClawReader<Key>::~OrganicClawReader() {
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, int8_t *val) {
	*val = static_cast<int8_t>(m_json[key].asInt());
	return OxError(0);
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, int16_t *val) {
	*val = static_cast<int16_t>(m_json[key].asInt());
	return OxError(0);
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, int32_t *val) {
	*val = static_cast<int32_t>(m_json[key].asInt());
	return OxError(0);
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, int64_t *val) {
	*val = static_cast<int64_t>(m_json[key].asInt64());
	return OxError(0);
}


template<typename Key>
Error OrganicClawReader<Key>::field(Key key, uint8_t *val) {
	*val = static_cast<uint8_t>(m_json[key].asUInt());
	return OxError(0);
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, uint16_t *val) {
	*val = static_cast<uint16_t>(m_json[key].asUInt());
	return OxError(0);
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, uint32_t *val) {
	*val = static_cast<uint32_t>(m_json[key].asUInt());
	return OxError(0);
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, uint64_t *val) {
	*val = static_cast<uint64_t>(m_json[key].asUInt64());
	return OxError(0);
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, bool *val) {
	*val = m_json[key].asBool();
	return OxError(0);
}

template<typename Key>
Error OrganicClawReader<Key>::field(Key key, SerStr val) {
	const char *begin = nullptr, *end = nullptr;
	m_json[key].getString(&begin, &end);
	auto strSize = end - begin;
	if (strSize >= val.cap()) {
		return OxError(1);
	}
	ox_memcpy(val.data(), begin, static_cast<std::size_t>(strSize));
	val.data()[strSize] = 0;
	return OxError(0);
}

template<typename Key>
[[nodiscard]] std::size_t OrganicClawReader<Key>::arrayLength(Key key, bool) {
	return m_json[key].size();
}

template<typename Key>
[[nodiscard]] std::size_t OrganicClawReader<Key>::stringLength(Key key) {
	const char *begin = nullptr, *end = nullptr;
	m_json[key].getString(&begin, &end);
	return static_cast<std::size_t>(end - begin);
}

template<typename Key>
void OrganicClawReader<Key>::setTypeInfo(const char*, int fields) {
	m_fields = fields;
	m_buffIt = static_cast<std::size_t>((fields / 8 + 1) - (fields % 8 == 0));
}

template<typename Key>
OrganicClawReader<Key> OrganicClawReader<Key>::child(Key key) {
	return OrganicClawReader<Key>(m_json[key]);
}

template class OrganicClawReader<const char*>;
template class OrganicClawReader<Json::ArrayIndex>;

}
