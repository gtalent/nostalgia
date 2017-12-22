/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/byteswap.hpp>
#include <ox/std/string.hpp>
#include "err.hpp"
#include "optype.hpp"
#include "presencemask.hpp"

namespace ox {

class MetalClawReader {

	private:
		typedef uint32_t ArrayLength;
		typedef uint32_t StringLength;

		FieldPresenseMask m_fieldPresence;
		int m_fields = 0;
		int m_field = 0;
		size_t m_buffIt = 0;
		size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;

	public:
		MetalClawReader(uint8_t *buff, size_t buffLen);

		int op(const char*, int8_t *val);
		int op(const char*, int16_t *val);
		int op(const char*, int32_t *val);
		int op(const char*, int64_t *val);

		int op(const char*, uint8_t *val);
		int op(const char*, uint16_t *val);
		int op(const char*, uint32_t *val);
		int op(const char*, uint64_t *val);

		int op(const char*, bool *val);

		template<typename T>
		int op(const char*, T *val, size_t len);

		template<typename T>
		int op(const char*, T *val);

		template<size_t L>
		int op(const char*, ox::bstring<L> *val);

		size_t arrayLength(const char*);

		// stringLength returns the length of the string, including the null terminator.
		size_t stringLength(const char*);

		void setFields(int fields);

      OpType opType() {
          return OpType::Read;
      }

	private:
		template<typename I>
		int readInteger(I *val);
};

template<typename T>
int MetalClawReader::op(const char*, T *val) {
	int err = 0;
	if (m_fieldPresence.get(m_field)) {
		MetalClawReader reader(m_buff + m_buffIt, m_buffLen - m_buffIt);
		err |= ioOp(&reader, val);
		m_buffIt += reader.m_buffIt;
	}
	m_field++;
	return err;
};

template<size_t L>
int MetalClawReader::op(const char*, ox::bstring<L> *val) {
	int err = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		size_t size = 0;
		if (m_buffIt + sizeof(StringLength) < m_buffLen) {
			size = ox::bigEndianAdapt(*((StringLength*) &m_buff[m_buffIt]));
			m_buffIt += sizeof(StringLength);
		} else {
			err |= MC_BUFFENDED;
		}

		// read the string
		if (val->cap() >= size) {
			if (m_buffIt + size < m_buffLen) {
				ox_memcpy(val, &m_buff[m_buffIt], size);
				m_buffIt += size;
			} else {
				err |= MC_BUFFENDED;
			}
		} else {
			err |= MC_OUTBUFFENDED;
		}
	} else {
		*val = "";
	}
	m_field++;
	return err;
};

template<typename I>
int MetalClawReader::readInteger(I *val) {
	int err = 0;
	if (m_fieldPresence.get(m_field)) {
		if (m_buffIt + sizeof(I) < m_buffLen) {
			*val = ox::bigEndianAdapt(*((I*) &m_buff[m_buffIt]));
			m_buffIt += sizeof(I);
		} else {
			err = MC_BUFFENDED;
		}
	} else {
		*val = 0;
	}
	m_field++;
	return err;
};

template<typename T>
int MetalClawReader::op(const char*, T *val, size_t valLen) {
	int err = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		size_t len = 0;
		if (m_buffIt + sizeof(ArrayLength) < m_buffLen) {
			len = ox::bigEndianAdapt(*((ArrayLength*) &m_buff[m_buffIt]));
			m_buffIt += sizeof(ArrayLength);
		} else {
			err = MC_BUFFENDED;
		}

		// read the list
		if (valLen >= len) {
			MetalClawReader reader(m_buff + m_buffIt, m_buffLen - m_buffIt);
			reader.setFields(len);
			for (size_t i = 0; i < len; i++) {
				err |= reader.op("", &val[i]);
			}
			m_buffIt += reader.m_buffIt;
		} else {
			err = MC_OUTBUFFENDED;
		}
	}
	m_field++;
	return err;
};

template<typename T>
int read(uint8_t *buff, size_t buffLen, T *val) {
	MetalClawReader reader(buff, buffLen);
	return ioOp(&reader, val);
}

}
