/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "write.hpp"

namespace ox {

OrganicClawWriter::OrganicClawWriter(int unionIdx): m_unionIdx(unionIdx) {
}

OrganicClawWriter::OrganicClawWriter(Json::Value json, int unionIdx):
	m_json(json),
	m_unionIdx(unionIdx) {
}

Error OrganicClawWriter::field(const char *key, int8_t *val) {
	if (*val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, int16_t *val) {
	if (*val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, int32_t *val) {
	if (*val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, int64_t *val) {
	if (*val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}


Error OrganicClawWriter::field(const char *key, uint8_t *val) {
	if (*val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, uint16_t *val) {
	if (targetValid() && *val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, uint32_t *val) {
	if (targetValid() && *val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, uint64_t *val) {
	if (targetValid() && *val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, bool *val) {
	if (targetValid() && *val) {
		value(key) = *val;
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, ox::String val) {
	if (targetValid() && val.len()) {
		value(key) = val.c_str();
	}
	++m_fieldIt;
	return OxError(0);
}

Error OrganicClawWriter::field(const char *key, SerStr val) {
	if (targetValid() && val.len()) {
		value(key) = val.c_str();
	}
	++m_fieldIt;
	return OxError(0);
}

Json::Value &OrganicClawWriter::value(const char *key) {
	if (m_json.isArray()) {
		return m_json[m_fieldIt];
	} else {
		return m_json[key];
	}
}

}
