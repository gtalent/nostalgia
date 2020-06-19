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
#include <ox/std/trace.hpp>

#include "write.hpp"

namespace ox {

MetalClawWriter::MetalClawWriter(uint8_t *buff, std::size_t buffLen, int unionIdx) noexcept:
	m_fieldPresence(buff, buffLen),
	m_unionIdx(unionIdx),
	m_buffLen(buffLen),
	m_buff(buff) {
}

MetalClawWriter::~MetalClawWriter() noexcept {
	if (m_field != m_fields) {
		oxTrace("ox::mc::MetalClawWriter::error") << "MetalClawReader: incorrect fields number given";
	}
}

Error MetalClawWriter::field(const char*, int8_t *val) noexcept {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, int16_t *val) noexcept {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, int32_t *val) noexcept {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, int64_t *val) noexcept {
	return appendInteger(*val);
}


Error MetalClawWriter::field(const char*, uint8_t *val) noexcept {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, uint16_t *val) noexcept {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, uint32_t *val) noexcept {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, uint64_t *val) noexcept {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, bool *val) noexcept {
	if (m_unionIdx == -1 || m_unionIdx == m_field) {
		oxReturnError(m_fieldPresence.set(m_field, *val));
	}
	++m_field;
	return OxError(0);
}

Error MetalClawWriter::field(const char*, SerStr val) noexcept {
	bool fieldSet = false;
	if (val.len() && (m_unionIdx == -1 || m_unionIdx == m_field)) {
		// write the length
		const auto strLen = mc::encodeInteger(val.len());
		if (m_buffIt + strLen.length + val.len() < m_buffLen) {
			ox_memcpy(&m_buff[m_buffIt], strLen.data, strLen.length);
			m_buffIt += strLen.length;
			// write the string
			ox_memcpy(&m_buff[m_buffIt], val.c_str(), val.len());
			m_buffIt += val.len();
			fieldSet = true;
		} else {
			return OxError(MC_BUFFENDED);
		}
	}
	oxReturnError(m_fieldPresence.set(m_field, fieldSet));
	++m_field;
	return OxError(0);
}

std::size_t MetalClawWriter::size() noexcept {
	return m_buffIt;
}

}
