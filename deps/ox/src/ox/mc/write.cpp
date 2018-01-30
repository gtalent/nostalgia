/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/byteswap.hpp>
#include <ox/std/memops.hpp>
#include "write.hpp"

namespace ox {

MetalClawWriter::MetalClawWriter(uint8_t *buff, size_t buffLen): m_fieldPresence(buff, buffLen) {
	m_buff = buff;
	m_buffLen = buffLen;
}

int MetalClawWriter::op(const char*, int8_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, int16_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, int32_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, int64_t *val) {
	return appendInteger(*val);
}


int MetalClawWriter::op(const char*, uint8_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, uint16_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, uint32_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, uint64_t *val) {
	return appendInteger(*val);
}

int MetalClawWriter::op(const char*, bool *val) {
	return m_fieldPresence.set(m_field++, *val);
}

void MetalClawWriter::setFields(int fields) {
	m_fields = fields;
	m_buffIt = (fields / 8 + 1) - (fields % 8 == 0);
	m_fieldPresence.setMaxLen(m_buffIt);
}

size_t MetalClawWriter::size() {
	return m_buffIt;
}

}
