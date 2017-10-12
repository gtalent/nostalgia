/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/string.hpp>
#include <ox/std/types.hpp>
#include "err.hpp"
#include "presencemask.hpp"

namespace ox {

class MetalClawWriter {

	private:
		FieldPresenseMask m_fieldPresence;
		int m_fields = 0;
		int m_field = 0;
		size_t m_buffIt = 0;
		size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;

	public:
		MetalClawWriter(uint8_t *buff, size_t buffLen);

		int op(const char*, int16_t *val);
		int op(const char*, int32_t *val);
		int op(const char*, int64_t *val);

		int op(const char*, uint16_t *val);
		int op(const char*, uint32_t *val);
		int op(const char*, uint64_t *val);

		int op(const char*, bool *val);

		template<typename T>
		int op(const char*, T *val, size_t len);

		template<size_t L>
		int op(const char*, ox::bstring<L> *val);

		template<typename T>
		int op(const char*, T *val);

		void setFields(int fields);

	private:
		template<typename I>
		int appendInteger(I val);
};

template<size_t L>
int MetalClawWriter::op(const char*, ox::bstring<L> *val) {
	int err = 0;
	bool fieldSet = false;
	if (val->len()) {
		// write the length
		typedef uint32_t StringLength;
		if (m_buffIt + sizeof(StringLength) + val->size() < m_buffLen) {
			*((StringLength*) &m_buff[m_buffIt]) = ox::bigEndianAdapt((StringLength) val->size());
			m_buffIt += sizeof(StringLength);

			// write the string
			ox_memcpy(&m_buff[m_buffIt], val, val->size());
			m_buffIt += val->size();
			fieldSet = true;
		} else {
			err = MC_BUFFENDED;
		}
	}
	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
};

template<typename T>
int MetalClawWriter::op(const char*, T *val) {
	int err = 0;
	bool fieldSet = false;
	MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
	err |= ioOp(&writer, val);
	if ((size_t) writer.m_fieldPresence.getMaxLen() < writer.m_buffIt) {
		m_buffIt += writer.m_buffIt;
		fieldSet = true;
	}
	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
};

template<typename I>
int MetalClawWriter::appendInteger(I val) {
	int err = 0;
	bool fieldSet = false;
	if (val) {
		if (m_buffIt + sizeof(I) < m_buffLen) {
			*((I*) &m_buff[m_buffIt]) = ox::bigEndianAdapt(val);
			fieldSet = true;
			m_buffIt += sizeof(I);
		} else {
			err |= MC_BUFFENDED;
		}
	}
	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
};

template<typename T>
int MetalClawWriter::op(const char*, T *val, size_t len) {
	int err = 0;
	bool fieldSet = false;

	if (len) {
		// write the length
		typedef uint32_t ArrayLength;
		if (m_buffIt + sizeof(ArrayLength) < m_buffLen) {
			*((T*) &m_buff[m_buffIt]) = ox::bigEndianAdapt((ArrayLength) len);
			m_buffIt += sizeof(ArrayLength);
		} else {
			err = MC_BUFFENDED;
		}

		MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
		writer.setFields(len);

		// write the array
		for (size_t i = 0; i < len; i++) {
			err |= writer.op("", &val[i]);
		}

		m_buffIt += writer.m_buffIt;
		fieldSet = true;
	}

	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
};

template<typename T>
int write(uint8_t *buff, size_t buffLen, T *val) {
	MetalClawWriter writer(buff, buffLen);
	return ioOp(&writer, val);
}

}
