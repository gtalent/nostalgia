/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/byteswap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

#include "err.hpp"
#include "optype.hpp"
#include "presencemask.hpp"
#include "types.hpp"

namespace ox {

class MetalClawReader {

	private:
		FieldPresenseMask m_fieldPresence;
		int m_fields = 0;
		int m_field = 0;
		std::size_t m_buffIt = 0;
		std::size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;

	public:
		MetalClawReader(uint8_t *buff, std::size_t buffLen);

		~MetalClawReader();

		int op(const char*, int8_t *val);
		int op(const char*, int16_t *val);
		int op(const char*, int32_t *val);
		int op(const char*, int64_t *val);

		int op(const char*, uint8_t *val);
		int op(const char*, uint16_t *val);
		int op(const char*, uint32_t *val);
		int op(const char*, uint64_t *val);

		int op(const char*, bool *val);

		/**
		 * Reads an array length from the current location in the buffer.
		 */
		[[nodiscard]] std::size_t arrayLength();

		template<typename T>
		int op(const char*, T *val, std::size_t len);

		template<typename T>
		int op(const char*, ox::Vector<T> *val);

		template<typename T>
		int op(const char*, T *val);

		template<std::size_t L>
		int op(const char*, ox::BString<L> *val);

		Error op(const char*, McStr val);

		std::size_t arrayLength(const char*);

		// stringLength returns the length of the string, including the null terminator.
		std::size_t stringLength(const char*);

		void setTypeInfo(const char *name, int fields);

		/**
		 * Returns a MetalClawReader to parse a child object.
		 */
		MetalClawReader child();

		/**
		 * Indicates whether or not the next field to be read is present.
		 */
		bool fieldPresent();

		/**
		 * Indicates whether or not the given field is present.
		 */
		bool fieldPresent(int fieldNo);

		/**
		 * @return the number of fields in this struct or list
		 */
		bool fields();

		static constexpr OpType opType() {
			return OpType::Read;
		}

	private:
		template<typename I>
		int readInteger(I *val);
};

template<typename T>
int MetalClawReader::op(const char*, T *val) {
	int err = 0;
	if (val && m_fieldPresence.get(m_field)) {
		MetalClawReader reader(m_buff + m_buffIt, m_buffLen - m_buffIt);
		err |= ioOp(&reader, val);
		m_buffIt += reader.m_buffIt;
	}
	m_field++;
	return err;
};

template<std::size_t L>
int MetalClawReader::op(const char *name, ox::BString<L> *val) {
	return op(name, McStr(val->data(), val->cap()));
}

template<typename I>
int MetalClawReader::readInteger(I *val) {
	int err = 0;
	if (m_fieldPresence.get(m_field)) {
		if (m_buffIt + sizeof(I) < m_buffLen) {
			*val = *reinterpret_cast<LittleEndian<I>*>(&m_buff[m_buffIt]);
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
int MetalClawReader::op(const char*, T *val, std::size_t valLen) {
	int err = 0;
	if (m_fieldPresence.get(m_field)) {
		// read the length
		std::size_t len = 0;
		if (m_buffIt + sizeof(ArrayLength) < m_buffLen) {
			len = *reinterpret_cast<LittleEndian<ArrayLength>*>(&m_buff[m_buffIt]);
			m_buffIt += sizeof(ArrayLength);
		} else {
			err = MC_BUFFENDED;
		}

		// read the list
		if (valLen >= len) {
			auto reader = child();
			reader.setTypeInfo("List", len);
			for (std::size_t i = 0; i < len; i++) {
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
int MetalClawReader::op(const char*, ox::Vector<T> *val) {
	return op(nullptr, val->data(), val->size());
}

template<typename T>
int readMC(uint8_t *buff, std::size_t buffLen, T *val) {
	MetalClawReader reader(buff, buffLen);
	return ioOp(&reader, val);
}

}
