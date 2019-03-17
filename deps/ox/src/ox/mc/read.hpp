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
#include <ox/std/byteswap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/vector.hpp>

#include "err.hpp"
#include "intops.hpp"
#include "presenceindicator.hpp"
#include "types.hpp"

namespace ox {

class MetalClawReader {

	private:
		FieldPresenceIndicator m_fieldPresence;
		int m_fields = 0;
		int m_field = 0;
		std::size_t m_buffIt = 0;
		std::size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;
		MetalClawReader *m_parent = nullptr;

	public:
		MetalClawReader(uint8_t *buff, std::size_t buffLen, MetalClawReader *parent = nullptr);

		~MetalClawReader();

		Error op(const char*, int8_t *val);
		Error op(const char*, int16_t *val);
		Error op(const char*, int32_t *val);
		Error op(const char*, int64_t *val);

		Error op(const char*, uint8_t *val);
		Error op(const char*, uint16_t *val);
		Error op(const char*, uint32_t *val);
		Error op(const char*, uint64_t *val);

		Error op(const char*, bool *val);

		// array handler
		template<typename T>
		Error op(const char*, T *val, std::size_t len);

		template<typename T>
		Error op(const char*, ox::Vector<T> *val);

		template<typename T>
		Error op(const char*, T *val);

		template<std::size_t L>
		Error op(const char*, ox::BString<L> *val);

		Error op(const char*, SerStr val);

		/**
		 * Reads an array length from the current location in the buffer.
		 * @param pass indicates that the parsing should iterate past the array length
		 */
		[[nodiscard]] ArrayLength arrayLength(bool pass = true);

		/**
		 * Reads an string length from the current location in the buffer.
		 */
		[[nodiscard]] StringLength stringLength();

		void setTypeInfo(const char *name, int fields);

		/**
		 * Returns a MetalClawReader to parse a child object.
		 */
		[[nodiscard]] MetalClawReader child();

		/**
		 * Indicates whether or not the next field to be read is present.
		 */
		bool fieldPresent() const;

		/**
		 * Indicates whether or not the given field is present.
		 */
		bool fieldPresent(int fieldNo) const;

		void nextField() noexcept;

		static constexpr OpType opType() {
			return OpType::Read;
		}

	private:
		template<typename I>
		Error readInteger(I *val);

};

template<typename T>
Error MetalClawReader::op(const char*, T *val) {
	Error err = 0;
	if (val && m_fieldPresence.get(m_field++)) {
		auto reader = child();
		err |= ioOp(&reader, val);
	}
	return err;
};

template<std::size_t L>
Error MetalClawReader::op(const char *name, ox::BString<L> *val) {
	return op(name, SerStr(val->data(), val->cap()));
}

template<typename I>
Error MetalClawReader::readInteger(I *val) {
	if (m_fieldPresence.get(m_field++)) {
		std::size_t bytesRead = 0;
		if (m_buffIt >= m_buffLen) {
			return OxError(MC_BUFFENDED);
		}
		auto valErr = mc::decodeInteger<I>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
		m_buffIt += bytesRead;
		oxReturnError(valErr.error);
		*val = valErr.value;
	} else {
		*val = 0;
	}
	return OxError(0);
};

// array handler
template<typename T>
Error MetalClawReader::op(const char*, T *val, std::size_t valLen) {
	Error err = 0;
	if (m_fieldPresence.get(m_field++)) {
		// read the length
		if (m_buffIt >= m_buffLen) {
			return OxError(MC_BUFFENDED);
		}
		std::size_t bytesRead = 0;
		auto len = mc::decodeInteger<StringLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
		m_buffIt += bytesRead;
		oxReturnError(len.error);

		// read the list
		if (valLen >= len) {
			auto reader = child();
			reader.setTypeInfo("List", len);
			for (std::size_t i = 0; i < len; i++) {
				err |= reader.op("", &val[i]);
			}
		} else {
			err = OxError(MC_OUTBUFFENDED);
		}
	}
	return err;
};

template<typename T>
Error MetalClawReader::op(const char*, ox::Vector<T> *val) {
	return op(nullptr, val->data(), val->size());
}

template<typename T>
Error readMC(uint8_t *buff, std::size_t buffLen, T *val) {
	MetalClawReader reader(buff, buffLen);
	return ioOp(&reader, val);
}

}
