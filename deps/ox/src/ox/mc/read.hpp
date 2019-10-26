/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/model/optype.hpp>
#include <ox/model/types.hpp>
#include <ox/std/byteswap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/trace.hpp>
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

		Error field(const char*, int8_t *val);
		Error field(const char*, int16_t *val);
		Error field(const char*, int32_t *val);
		Error field(const char*, int64_t *val);

		Error field(const char*, uint8_t *val);
		Error field(const char*, uint16_t *val);
		Error field(const char*, uint32_t *val);
		Error field(const char*, uint64_t *val);

		Error field(const char*, bool *val);

		// array handler
		template<typename T>
		Error field(const char*, T *val, std::size_t len);

		template<typename T>
		Error field(const char*, ox::Vector<T> *val);

		template<typename T>
		Error field(const char*, T *val);

		template<std::size_t L>
		Error field(const char*, ox::BString<L> *val);

		Error field(const char*, SerStr val);

		/**
		 * Reads an array length from the current location in the buffer.
		 * @param pass indicates that the parsing should iterate past the array length
		 */
		[[nodiscard]] ValErr<ArrayLength> arrayLength(bool pass = true);

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
Error MetalClawReader::field(const char*, T *val) {
	if (val && m_fieldPresence.get(m_field++)) {
		auto reader = child();
		oxReturnError(model(&reader, val));
	}
	return OxError(0);
}

template<std::size_t L>
Error MetalClawReader::field(const char *name, ox::BString<L> *val) {
	return field(name, SerStr(val->data(), val->cap()));
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
}

// array handler
template<typename T>
Error MetalClawReader::field(const char *name, T *val, std::size_t valLen) {
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
		if (valLen >= len.value) {
			auto reader = child();
			reader.setTypeInfo("List", len.value);
			for (std::size_t i = 0; i < len.value; i++) {
				oxReturnError(reader.field("", &val[i]));
			}
		} else {
			oxTrace("ox::mc::read::field(T)") << name << ", size:" << valLen;
			return OxError(MC_OUTBUFFENDED);
		}
	}
	return OxError(0);
}

template<typename T>
Error MetalClawReader::field(const char* name, ox::Vector<T> *val) {
	const auto [len, err] = arrayLength(false);
	oxReturnError(err);
	val->resize(len);
	return field(name, val->data(), val->size());
}

template<typename T>
Error readMC(uint8_t *buff, std::size_t buffLen, T *val) {
	MetalClawReader reader(buff, buffLen);
	return model(&reader, val);
}

}
