/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/assert.hpp>
#include <ox/std/byteswap.hpp>
#include <ox/std/memops.hpp>

#include "read.hpp"

namespace ox {

MetalClawReader::MetalClawReader(uint8_t *buff, std::size_t buffLen, MetalClawReader *parent): m_fieldPresence(buff, buffLen) {
	m_buff = buff;
	m_buffLen = buffLen;
	m_parent = parent;
}

MetalClawReader::~MetalClawReader() {
	if (m_parent) {
		m_parent->m_buffIt += m_buffIt;
	}
	oxAssert(m_field == m_fields, "MetalClawReader: incorrect fields number given");
}

Error MetalClawReader::field(const char*, int8_t *val) {
	return readInteger(val);
}

Error MetalClawReader::field(const char*, int16_t *val) {
	return readInteger(val);
}

Error MetalClawReader::field(const char*, int32_t *val) {
	return readInteger(val);
}

Error MetalClawReader::field(const char*, int64_t *val) {
	return readInteger(val);
}


Error MetalClawReader::field(const char*, uint8_t *val) {
	return readInteger(val);
}

Error MetalClawReader::field(const char*, uint16_t *val) {
	return readInteger(val);
}

Error MetalClawReader::field(const char*, uint32_t *val) {
	return readInteger(val);
}

Error MetalClawReader::field(const char*, uint64_t *val) {
	return readInteger(val);
}

Error MetalClawReader::field(const char*, bool *val) {
	auto valErr = m_fieldPresence.get(m_field++);
	*val = valErr.value;
	return OxError(valErr.error);
}

Error MetalClawReader::field(const char*, SerStr val) {
	if (m_fieldPresence.get(m_field++)) {
		// read the length
		if (m_buffIt >= m_buffLen) {
			return OxError(MC_BUFFENDED);
		}
		std::size_t bytesRead = 0;
		auto [size, err] = mc::decodeInteger<StringLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
		m_buffIt += bytesRead;
		oxReturnError(err);

		// read the string
		if (val.cap() > -1 && static_cast<StringLength>(val.cap()) >= size) {
			if (m_buffIt + size < m_buffLen) {
				ox_memcpy(val.data(), &m_buff[m_buffIt], size);
				m_buffIt += size;
			} else {
				return OxError(MC_BUFFENDED);
			}
		} else {
			return OxError(MC_OUTBUFFENDED);
		}
	} else {
		val.data()[0] = 0;
	}
	return OxError(0);
}

[[nodiscard]] ArrayLength MetalClawReader::arrayLength(bool pass) {
	std::size_t len = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		if (m_buffIt >= m_buffLen) {
			return OxError(MC_BUFFENDED);
		}
		std::size_t bytesRead = 0;
		auto [len, _] = mc::decodeInteger<StringLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
		if (pass) {
			m_buffIt += sizeof(ArrayLength);
		}
	}
	return len;
}

[[nodiscard]] StringLength MetalClawReader::stringLength() {
	if (m_fieldPresence.get(m_field)) {
		// read the length
		std::size_t bytesRead = 0;
		auto [len, _] = mc::decodeInteger<StringLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
		return len;
	}
	return 0;
}

void MetalClawReader::setTypeInfo(const char*, int fields) {
	m_fields = fields;
	m_buffIt = (fields / 8 + 1) - (fields % 8 == 0);
	m_fieldPresence.setFields(fields);
	m_fieldPresence.setMaxLen(m_buffIt);
}

MetalClawReader MetalClawReader::child() {
	return MetalClawReader(m_buff + m_buffIt, m_buffLen - m_buffIt, this);
}

bool MetalClawReader::fieldPresent() const {
	return m_fieldPresence.get(m_field).value;
}

bool MetalClawReader::fieldPresent(int fieldNo) const {
	return m_fieldPresence.get(fieldNo).value;
}

void MetalClawReader::nextField() noexcept {
	++m_field;
}

}
