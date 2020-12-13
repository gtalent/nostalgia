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

OrganicClawReader::OrganicClawReader(const uint8_t *buff, std::size_t buffSize) {
	auto json = bit_cast<const char*>(buff);
	auto jsonLen = ox_strnlen(json, buffSize);
	Json::CharReaderBuilder parserBuilder;
	auto parser = std::unique_ptr<Json::CharReader>(parserBuilder.newCharReader());
	if (!parser->parse(json, json + jsonLen, &m_json, nullptr)) {
		throw OxError(1, "Could not parse JSON");
	}
}

OrganicClawReader::OrganicClawReader(const char *json, std::size_t jsonLen) {
	Json::CharReaderBuilder parserBuilder;
	auto parser = std::unique_ptr<Json::CharReader>(parserBuilder.newCharReader());
	if (!parser->parse(json, json + jsonLen, &m_json, nullptr)) {
		throw OxError(1, "Could not parse JSON");
	}
}

OrganicClawReader::OrganicClawReader(const Json::Value &json, int unionIdx):
	m_json(json),
	m_unionIdx(unionIdx) {
}

Error OrganicClawReader::field(const char *key, int8_t *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = 0;
		} else if (jv.isInt()) {
			*val = static_cast<int8_t>(jv.asInt());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Error OrganicClawReader::field(const char *key, int16_t *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = 0;
		} else if (jv.isInt()) {
			*val = static_cast<int16_t>(jv.asInt());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Error OrganicClawReader::field(const char *key, int32_t *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = 0;
		} else if (jv.isInt()) {
			*val = static_cast<int32_t>(jv.asInt());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Error OrganicClawReader::field(const char *key, int64_t *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = 0;
		} else if (jv.isInt() || jv.isInt64()) {
			*val = static_cast<int64_t>(jv.asInt64());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}


Error OrganicClawReader::field(const char *key, uint8_t *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = 0;
		} else if (jv.isUInt()) {
			*val = static_cast<uint8_t>(jv.asUInt());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Error OrganicClawReader::field(const char *key, uint16_t *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = 0;
		} else if (jv.isUInt()) {
			*val = static_cast<uint16_t>(jv.asUInt());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Error OrganicClawReader::field(const char *key, uint32_t *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = 0;
		} else if (jv.isUInt()) {
			*val = static_cast<uint32_t>(jv.asUInt());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Error OrganicClawReader::field(const char *key, uint64_t *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = 0;
		} else if (jv.isUInt() || jv.isUInt64()) {
			*val = static_cast<uint64_t>(jv.asUInt64());
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Error OrganicClawReader::field(const char *key, bool *val) {
	auto err = OxError(0);
	if (targetValid()) {
		const auto &jv = value(key);
		if (jv.empty()) {
			*val = false;
		} else if (jv.isBool()) {
			*val = jv.asBool();
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Error OrganicClawReader::field(const char *key, SerStr val) {
	auto err = OxError(0);
	const char *begin = nullptr, *end = nullptr;
	const auto &jv = value(key);
	if (targetValid()) {
		if (jv.empty()) {
			auto data = val.data();
			if (data) {
				data[0] = 0;
			}
		} else if (jv.isString()) {
			jv.getString(&begin, &end);
			auto strSize = end - begin;
			auto data = val.data(static_cast<std::size_t>(strSize) + 1);
			if (strSize >= val.cap()) {
				err = OxError(1, "String size exceeds capacity of destination");
			} else {
				ox_memcpy(data, begin, static_cast<std::size_t>(strSize));
				data[strSize] = 0;
			}
		} else {
			err = OxError(1, "Type mismatch");
		}
	}
	++m_fieldIt;
	return err;
}

Result<std::size_t> OrganicClawReader::arrayLength(const char *key, bool) {
	const auto &jv = value(key);
	if (jv.empty()) {
		return 0;
	}
	if (jv.isArray()) {
		return jv.size();
	}
	return OxError(1, "Type mismatch");
}

[[nodiscard]] std::size_t OrganicClawReader::stringLength(const char *key) {
	const char *begin = nullptr, *end = nullptr;
	const auto &jv = value(key);
	if (jv.empty()) {
		return 0;
	}
	if (jv.isString()) {
		jv.getString(&begin, &end);
		return static_cast<std::size_t>(end - begin);
	}
	return OxError(1, "Type mismatch");
}

OrganicClawReader OrganicClawReader::child(const char *key, int unionIdx) {
	return OrganicClawReader(m_json[key], unionIdx);
}

bool OrganicClawReader::fieldPresent(const char *key) {
	return !m_json[key].empty();
}

Json::Value &OrganicClawReader::value(const char *key) {
	if (m_json.isArray()) {
		return m_json[m_fieldIt];
	} else {
		return m_json[key];
	}
}

bool OrganicClawReader::targetValid() noexcept {
	return static_cast<int>(m_fieldIt) == m_unionIdx || m_unionIdx == -1;
}

}
