/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/ser/optype.hpp>
#include <ox/ser/types.hpp>
#include <ox/std/bitops.hpp>
#include <ox/std/byteswap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/types.hpp>
#include <ox/std/vector.hpp>

#include "intops.hpp"
#include "err.hpp"
#include "presenceindicator.hpp"
#include "types.hpp"

namespace ox {

class MetalClawWriter {

	private:
		FieldPresenceIndicator m_fieldPresence;
		int m_fields = 0;
		int m_field = 0;
		std::size_t m_buffIt = 0;
		std::size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;

	public:
		MetalClawWriter(uint8_t *buff, std::size_t buffLen);

		~MetalClawWriter() noexcept;

		Error op(const char*, int8_t *val);
		Error op(const char*, int16_t *val);
		Error op(const char*, int32_t *val);
		Error op(const char*, int64_t *val);

		Error op(const char*, uint8_t *val);
		Error op(const char*, uint16_t *val);
		Error op(const char*, uint32_t *val);
		Error op(const char*, uint64_t *val);

		Error op(const char*, bool *val);

		template<typename T>
		Error op(const char*, T *val, std::size_t len);

		template<typename T>
		Error op(const char*, ox::Vector<T> *val);

		template<std::size_t L>
		Error op(const char*, ox::BString<L> *val);

		Error op(const char*, SerStr val);

		template<typename T>
		Error op(const char*, T *val);

		void setTypeInfo(const char *name, int fields);

		std::size_t size();

		static constexpr OpType opType() {
			return OpType::Write;
		}

	private:
		template<typename I>
		Error appendInteger(I val);
};

template<std::size_t L>
Error MetalClawWriter::op(const char *name, ox::BString<L> *val) {
	return op(name, SerStr(val->data(), val->cap()));
}

template<typename T>
Error MetalClawWriter::op(const char*, T *val) {
	int err = 0;
	bool fieldSet = false;
	MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
	if (val) {
		err |= ioOp(&writer, val);
		if (static_cast<std::size_t>(writer.m_fieldPresence.getMaxLen()) < writer.m_buffIt) {
			m_buffIt += writer.m_buffIt;
			fieldSet = true;
		}
	}
	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
}

template<typename T>
Error MetalClawWriter::op(const char*, ox::Vector<T> *val) {
	return op(nullptr, val->data(), val->size());
}

template<typename T>
Error MetalClawWriter::op(const char*, T *val, std::size_t len) {
	Error err = 0;
	bool fieldSet = false;

	if (len) {
		// write the length
		if (m_buffIt + sizeof(ArrayLength) < m_buffLen) {
			*reinterpret_cast<LittleEndian<ArrayLength>*>(&m_buff[m_buffIt]) = static_cast<ArrayLength>(len);
			m_buffIt += sizeof(ArrayLength);
		} else {
			err = MC_BUFFENDED;
		}

		MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
		writer.setTypeInfo("List", len);

		// write the array
		for (std::size_t i = 0; i < len; i++) {
			err |= writer.op("", &val[i]);
		}

		m_buffIt += writer.m_buffIt;
		fieldSet = true;
	}

	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
}

template<typename I>
Error MetalClawWriter::appendInteger(I val) {
	Error err = 0;
	bool fieldSet = false;
	if (val) {
		if (m_buffIt + sizeof(I) < m_buffLen) {
			LittleEndian<I> leVal = val;
			mc::encodeInteger(val);
			// bits needed to represent number factoring in space possibly needed
			// for signed bit
			const auto bits = mc::highestBit(val) + (ox::is_signed<I> ? 1 : 0) / 8;
			// bytes needed to store value
			std::size_t bytes = bits / 8 + (bits % 8 != 0);
			const auto bytesIndicator = onMask<uint8_t>(bytes - 1) << (7 - bytes);
			// factor in bits needed for bytesIndicator (does not affect bytesIndicator)
			// bits for integer + bits needed to represent bytes > bits available
			if (bits + bytes > bytes * 8) {
				++bytes;
			}

			if (bytes == 9) {
				m_buff[m_buffIt++] = bytesIndicator;
				
			} else {
			}
			*reinterpret_cast<LittleEndian<I>*>(&m_buff[m_buffIt]) = leVal;
			fieldSet = true;
			m_buffIt += sizeof(I);
		} else {
			err |= MC_BUFFENDED;
		}
	}
	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
}

template<typename T>
Error writeMC(uint8_t *buff, std::size_t buffLen, T *val, std::size_t *sizeOut = nullptr) {
	MetalClawWriter writer(buff, buffLen);
	auto err = ioOp(&writer, val);
	if (sizeOut) {
		*sizeOut = writer.size();
	}
	return err;
}

}
