/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/byteswap.hpp>
#include <ox/std/memops.hpp>
#include "read.hpp"

namespace ox {

MetalClawReader::MetalClawReader(uint8_t *buff, size_t buffLen): m_fieldPresence(buff, buffLen) {
	m_buff = buff;
	m_buffLen = buffLen;
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

size_t MetalClawReader::arrayLength(const char*) {
	size_t len = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		if (m_buffIt + sizeof(ArrayLength) < m_buffLen) {
			len = *reinterpret_cast<LittleEndian<ArrayLength>*>(&m_buff[m_buffIt]);
		}
	}
	return len;
}

size_t MetalClawReader::stringLength(const char*) {
	size_t len = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		if (m_buffIt + sizeof(StringLength) < m_buffLen) {
			len = *reinterpret_cast<LittleEndian<StringLength>*>(&m_buff[m_buffIt]);
		}
	}
	return len;
}

void MetalClawReader::setFields(int fields) {
	m_fields = fields;
	m_buffIt = (fields / 8 + 1) - (fields % 8 == 0);
	m_fieldPresence.setMaxLen(m_buffIt);
}

}
