/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "write.hpp"

namespace ox {

template<typename Key>
OrganicClawWriter<Key>::OrganicClawWriter(Json::Value json) {
	m_json = json;
}

template<typename Key>
OrganicClawWriter<Key>::~OrganicClawWriter() {
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, int8_t *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, int16_t *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, int32_t *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, int64_t *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}


template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, uint8_t *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, uint16_t *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, uint32_t *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, uint64_t *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, bool *val) {
	if (*val) {
		m_json[key] = *val;
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, ox::String val) {
	if (val.len()) {
		m_json[key] = val.c_str();
	}
	return OxError(0);
}

template<typename Key>
Error OrganicClawWriter<Key>::field(Key key, SerStr val) {
	if (val.len()) {
		m_json[key] = val.c_str();
	}
	return OxError(0);
}

template<typename Key>
void OrganicClawWriter<Key>::setTypeInfo(const char*, int fields) {
	m_fields = fields;
}

template class OrganicClawWriter<const char*>;
template class OrganicClawWriter<Json::ArrayIndex>;

}
