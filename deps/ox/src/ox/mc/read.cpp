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

MetalClawReader::MetalClawReader(uint8_t *buff, std::size_t buffLen): m_fieldPresence(buff, buffLen) {
	m_buff = buff;
	m_buffLen = buffLen;
}

MetalClawReader::~MetalClawReader() {
	oxAssert(m_field == m_fields, "MetalClawReader: incorrect fields number given");
}

int MetalClawReader::op(const char*, int8_t *val) {
	return readInteger(val);
}

int MetalClawReader::op(const char*, int16_t *val) {
	return readInteger(val);
}

int MetalClawReader::op(const char*, int32_t *val) {
	return readInteger(val);
}

int MetalClawReader::op(const char*, int64_t *val) {
	return readInteger(val);
}


int MetalClawReader::op(const char*, uint8_t *val) {
	return readInteger(val);
}

int MetalClawReader::op(const char*, uint16_t *val) {
	return readInteger(val);
}

int MetalClawReader::op(const char*, uint32_t *val) {
	return readInteger(val);
}

int MetalClawReader::op(const char*, uint64_t *val) {
	return readInteger(val);
}

int MetalClawReader::op(const char*, bool *val) {
	*val = m_fieldPresence.get(m_field++);
	return 0;
}

Error MetalClawReader::op(const char*, McStr val) {
	int err = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		int size = 0;
		if (m_buffIt + sizeof(StringLength) < m_buffLen) {
			size = *reinterpret_cast<LittleEndian<StringLength>*>(&m_buff[m_buffIt]);
			m_buffIt += sizeof(StringLength);
		} else {
			err |= MC_BUFFENDED;
		}

		// read the string
		if (val.cap() >= size) {
			if (m_buffIt + size < m_buffLen) {
				ox_memcpy(val.data(), &m_buff[m_buffIt], size);
				m_buffIt += size;
			} else {
				err |= MC_BUFFENDED;
			}
		} else {
			err |= MC_OUTBUFFENDED;
		}
	} else {
		val.data()[0] = 0;
	}
	m_field++;
	return err;
}

std::size_t MetalClawReader::arrayLength(const char*) {
	std::size_t len = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		if (m_buffIt + sizeof(ArrayLength) < m_buffLen) {
			len = *reinterpret_cast<LittleEndian<ArrayLength>*>(&m_buff[m_buffIt]);
		}
	}
	return len;
}

std::size_t MetalClawReader::stringLength(const char*) {
	std::size_t len = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		if (m_buffIt + sizeof(StringLength) < m_buffLen) {
			len = *reinterpret_cast<LittleEndian<StringLength>*>(&m_buff[m_buffIt]);
		}
	}
	return len;
}

void MetalClawReader::setTypeInfo(const char*, int fields) {
	m_fields = fields;
	m_buffIt = (fields / 8 + 1) - (fields % 8 == 0);
	m_fieldPresence.setMaxLen(m_buffIt);
}

MetalClawReader MetalClawReader::child() {
	return MetalClawReader(m_buff + m_buffIt, m_buffLen - m_buffIt);
}

bool MetalClawReader::fieldPresent() {
	return m_fieldPresence.get(m_field);
}

bool MetalClawReader::fieldPresent(int fieldNo) {
	return m_fieldPresence.get(fieldNo);
}

}
