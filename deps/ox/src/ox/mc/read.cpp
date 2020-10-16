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

MetalClawReader::MetalClawReader(uint8_t *buff, std::size_t buffLen, int unionIdx, MetalClawReader *parent) noexcept:
	m_fieldPresence(buff, buffLen),
	m_unionIdx(unionIdx),
	m_buffLen(buffLen),
	m_buff(buff),
	m_parent(parent) {
}

MetalClawReader::~MetalClawReader() noexcept {
	if (m_parent) {
		m_parent->m_buffIt += m_buffIt;
	}
	if (m_field != m_fields) {
		oxTrace("ox::mc::MetalClawReader::error") << "MetalClawReader: incorrect fields number given";
	}
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
	if (m_unionIdx == -1 || m_unionIdx == m_field) {
		auto valErr = m_fieldPresence.get(m_field);
		*val = valErr.value;
		oxReturnError(valErr.error);
	}
	++m_field;
	return OxError(0);
}

Error MetalClawReader::field(const char*, SerStr val) {
	if (m_unionIdx == -1 || m_unionIdx == m_field) {
		if (m_fieldPresence.get(m_field)) {
			// read the length
			if (m_buffIt >= m_buffLen) {
				return OxError(MC_BUFFENDED);
			}
			std::size_t bytesRead = 0;
			auto [size, err] = mc::decodeInteger<StringLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
			m_buffIt += bytesRead;
			oxReturnError(err);
			auto data = val.data(size + 1);
			// read the string
			if (val.cap() > -1 && static_cast<StringLength>(val.cap()) >= size) {
				if (m_buffIt + size <= m_buffLen) {
					ox_memcpy(data, &m_buff[m_buffIt], size);
					data[size] = 0;
					m_buffIt += size;
				} else {
					return OxError(MC_BUFFENDED);
				}
			} else {
				return OxError(MC_OUTBUFFENDED);
			}
		} else {
			auto data = val.data();
			if (data) {
				data[0] = 0;
			}
		}
	}
	++m_field;
	return OxError(0);
}

ValErr<ArrayLength> MetalClawReader::arrayLength(const char*, bool pass) {
	if ((m_unionIdx == -1 || m_unionIdx == m_field) && m_fieldPresence.get(m_field)) {
		// read the length
		if (m_buffIt >= m_buffLen) {
			return OxError(MC_BUFFENDED);
		}
		std::size_t bytesRead = 0;
		auto out = mc::decodeInteger<ArrayLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead).value;
		if (pass) {
			m_buffIt += bytesRead;
		}
		return out;
	}
	return OxError(1);
}

[[nodiscard]] StringLength MetalClawReader::stringLength(const char*) {
	if ((m_unionIdx == -1 || m_unionIdx == m_field) && m_fieldPresence.get(m_field)) {
		// read the length
		std::size_t bytesRead = 0;
		auto len = mc::decodeInteger<StringLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
		return len.value;
	}
	return 0;
}

MetalClawReader MetalClawReader::child(const char*, int unionIdx) {
	return MetalClawReader(m_buff + m_buffIt, m_buffLen - m_buffIt, unionIdx, this);
}

bool MetalClawReader::fieldPresent(const char*) const {
	return m_fieldPresence.get(m_field).value;
}

bool MetalClawReader::fieldPresent(int fieldNo) const {
	return m_fieldPresence.get(fieldNo).value;
}

void MetalClawReader::nextField() noexcept {
	++m_field;
}

}
