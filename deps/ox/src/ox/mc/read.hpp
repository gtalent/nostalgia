/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
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
		int m_unionIdx = -1;
		std::size_t m_buffIt = 0;
		std::size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;
		MetalClawReader *m_parent = nullptr;

	public:
		MetalClawReader(uint8_t *buff, std::size_t buffLen, int unionIdx = -1, MetalClawReader *parent = nullptr) noexcept;

		~MetalClawReader() noexcept;

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

		// map handler
		template<typename T>
		Error field(const char*, HashMap<String, T> *val);

		// array handler, with callback to allow handling individual elements
		template<typename T, typename Handler>
		Error field(const char*, Handler handler);

		template<typename T>
		Error field(const char*, ox::Vector<T> *val);

		template<typename T>
		Error field(const char*, T *val);

		template<typename U>
		Error field(const char*, UnionView<U> val);

		template<std::size_t L>
		Error field(const char*, ox::BString<L> *val);

		Error field(const char*, SerStr val);

		/**
		 * Reads an array length from the current location in the buffer.
		 * @param pass indicates that the parsing should iterate past the array length
		 */
		Result<ArrayLength> arrayLength(const char *name, bool pass = true);

		/**
		 * Reads an string length from the current location in the buffer.
		 */
		[[nodiscard]] StringLength stringLength(const char *name);

		template<typename T = std::nullptr_t>
		void setTypeInfo(const char *name = T::TypeName, int fields = T::Fields);

		/**
		 * Returns a MetalClawReader to parse a child object.
		 */
		[[nodiscard]] MetalClawReader child(const char *name, int unionIdx = -1);

		/**
		 * Indicates whether or not the next field to be read is present.
		 */
		bool fieldPresent(const char *name) const;

		/**
		 * Indicates whether or not the given field is present.
		 */
		bool fieldPresent(int fieldNo) const;

		void nextField() noexcept;

		static constexpr auto opType() {
			return OpType::Read;
		}

	private:
		template<typename I>
		Error readInteger(I *val);

};

template<typename T>
Error MetalClawReader::field(const char*, T *val) {
	if ((m_unionIdx == -1 || m_unionIdx == m_field) && val && m_fieldPresence.get(m_field)) {
		auto reader = child("");
		oxReturnError(model(&reader, val));
	}
	++m_field;
	return OxError(0);
}

template<typename U>
Error MetalClawReader::field(const char*, UnionView<U> val) {
	if ((m_unionIdx == -1 || m_unionIdx == m_field) && val.get() && m_fieldPresence.get(m_field)) {
		auto reader = child("", val.idx());
		oxReturnError(model(&reader, val.get()));
	}
	++m_field;
	return OxError(0);
}

template<std::size_t L>
Error MetalClawReader::field(const char *name, ox::BString<L> *val) {
	return field(name, SerStr(val->data(), val->cap()));
}

template<typename I>
Error MetalClawReader::readInteger(I *val) {
	if (m_unionIdx == -1 || m_unionIdx == m_field) {
		if (m_fieldPresence.get(m_field)) {
			std::size_t bytesRead = 0;
			if (m_buffIt >= m_buffLen) {
				oxTrace("ox::MetalClaw::readInteger") << "Buffer ended";
				return OxError(MC_BUFFENDED);
			}
			auto valErr = mc::decodeInteger<I>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
			m_buffIt += bytesRead;
			oxReturnError(valErr.error);
			*val = valErr.value;
		} else {
			*val = 0;
		}
	}
	++m_field;
	return OxError(0);
}

// array handler
template<typename T>
Error MetalClawReader::field(const char *name, T *val, std::size_t valLen) {
	if (m_unionIdx == -1 || m_unionIdx == m_field) {
		if (m_fieldPresence.get(m_field)) {
			// read the length
			if (m_buffIt >= m_buffLen) {
				return OxError(MC_BUFFENDED);
			}
			std::size_t bytesRead = 0;
			auto len = mc::decodeInteger<ArrayLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
			m_buffIt += bytesRead;
			oxReturnError(len.error);

			// read the list
			if (valLen >= len.value) {
				auto reader = child("");
				reader.setTypeInfo("List", len.value);
				for (std::size_t i = 0; i < len.value; i++) {
					oxReturnError(reader.field("", &val[i]));
				}
			} else {
				oxTrace("ox::mc::read::field(T)") << name << ", size:" << valLen;
				return OxError(MC_OUTBUFFENDED);
			}
		}
	}
	++m_field;
	return OxError(0);
}

template<typename T>
Error MetalClawReader::field(const char*, HashMap<String, T> *val) {
	if (m_unionIdx == -1 || m_unionIdx == m_field) {
		if (m_fieldPresence.get(m_field)) {
			// read the length
			if (m_buffIt >= m_buffLen) {
				return OxError(MC_BUFFENDED);
			}
			std::size_t bytesRead = 0;
			auto len = mc::decodeInteger<ArrayLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
			m_buffIt += bytesRead;
			oxReturnError(len.error);

			// read the list
			auto reader = child("");
			reader.setTypeInfo("List", len.value);
			for (std::size_t i = 0; i < len.value; i++) {
				auto keyLen = reader.stringLength(nullptr);
				auto wkey = ox_malloca(keyLen + 1, char, 0);
				oxReturnError(reader.field("", SerStr(wkey.get(), keyLen)));
				oxReturnError(reader.field("", &val->at(wkey.get())));
			}
		}
	}
	++m_field;
	return OxError(0);
}

template<typename T, typename Handler>
Error MetalClawReader::field(const char*, Handler handler) {
	if (m_unionIdx == -1 || m_unionIdx == m_field) {
		if (m_fieldPresence.get(m_field)) {
			// read the length
			if (m_buffIt >= m_buffLen) {
				return OxError(MC_BUFFENDED);
			}
			std::size_t bytesRead = 0;
			auto len = mc::decodeInteger<ArrayLength>(&m_buff[m_buffIt], m_buffLen - m_buffIt, &bytesRead);
			m_buffIt += bytesRead;
			oxReturnError(len.error);

			// read the list
			auto reader = child("");
			reader.setTypeInfo("List", len.value);
			for (std::size_t i = 0; i < len.value; i++) {
				T val;
				oxReturnError(reader.field("", &val));
				oxReturnError(handler(i, &val));
			}
		}
	}
	++m_field;
	return OxError(0);
}

template<typename T>
Error MetalClawReader::field(const char* name, ox::Vector<T> *val) {
	if (m_unionIdx == -1 || m_unionIdx == m_field) {
		// set size of val if the field is present, don't worry about it if not
		if (m_fieldPresence.get(m_field)) {
			const auto [len, err] = arrayLength(name, false);
			oxReturnError(err);
			val->resize(len);
		}
		return field(name, val->data(), val->size());
	}
	return OxError(0);
}

template<typename T>
void MetalClawReader::setTypeInfo(const char*, int fields) {
	m_fields = fields;
	m_buffIt = (fields / 8 + 1) - (fields % 8 == 0);
	m_fieldPresence.setFields(fields);
	m_fieldPresence.setMaxLen(m_buffIt);
}

template<typename T>
Error readMC(uint8_t *buff, std::size_t buffLen, T *val) {
	MetalClawReader reader(buff, buffLen);
	return model(&reader, val);
}

}
