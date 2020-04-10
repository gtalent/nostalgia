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

#include "write.hpp"

namespace ox {

MetalClawWriter::MetalClawWriter(uint8_t *buff, std::size_t buffLen): m_fieldPresence(buff, buffLen) {
	m_buff = buff;
	m_buffLen = buffLen;
}

MetalClawWriter::~MetalClawWriter() noexcept {
	oxAssert(m_field == m_fields, "MetalClawWriter: incorrect fields number given");
}

Error MetalClawWriter::field(const char*, int8_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, int16_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, int32_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, int64_t *val) {
	return appendInteger(*val);
}


Error MetalClawWriter::field(const char*, uint8_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, uint16_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, uint32_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, uint64_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::field(const char*, bool *val) {
	return m_fieldPresence.set(m_field++, *val);
}

Error MetalClawWriter::field(const char*, SerStr val) {
	auto err = OxError(0);
	bool fieldSet = false;
	if (val.cap()) {
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
			err = OxError(MC_BUFFENDED);
		}
	}
	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
}

void MetalClawWriter::setTypeInfo(const char*, int fields) {
	m_fields = fields;
	m_buffIt = (fields / 8 + 1) - (fields % 8 == 0);
	m_fieldPresence.setFields(fields);
	m_fieldPresence.setMaxLen(m_buffIt);
}

std::size_t MetalClawWriter::size() {
	return m_buffIt;
}

}
