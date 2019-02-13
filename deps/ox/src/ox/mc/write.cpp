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

Error MetalClawWriter::op(const char*, int8_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::op(const char*, int16_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::op(const char*, int32_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::op(const char*, int64_t *val) {
	return appendInteger(*val);
}


Error MetalClawWriter::op(const char*, uint8_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::op(const char*, uint16_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::op(const char*, uint32_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::op(const char*, uint64_t *val) {
	return appendInteger(*val);
}

Error MetalClawWriter::op(const char*, bool *val) {
	return m_fieldPresence.set(m_field++, *val);
}

Error MetalClawWriter::op(const char*, McStr val) {
	int err = 0;
	bool fieldSet = false;
	if (val.cap()) {
		// write the length
		typedef uint32_t StringLength;
		if (m_buffIt + sizeof(StringLength) + val.bytes() < m_buffLen) {
			*reinterpret_cast<LittleEndian<StringLength>*>(&m_buff[m_buffIt]) = static_cast<StringLength>(val.bytes());
			m_buffIt += sizeof(StringLength);

			// write the string
			ox_memcpy(&m_buff[m_buffIt], val.c_str(), val.bytes());
			m_buffIt += val.bytes();
			fieldSet = true;
		} else {
			err = MC_BUFFENDED;
		}
	}
	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
}

void MetalClawWriter::setTypeInfo(const char*, int fields) {
	m_fields = fields;
	m_buffIt = (fields / 8 + 1) - (fields % 8 == 0);
	m_fieldPresence.setMaxLen(m_buffIt);
}

std::size_t MetalClawWriter::size() {
	return m_buffIt;
}

}
