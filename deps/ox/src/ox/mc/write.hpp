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
#include <ox/std/bit.hpp>
#include <ox/std/byteswap.hpp>
#include <ox/std/string.hpp>
#include <ox/std/types.hpp>
#include <ox/std/units.hpp>
#include <ox/std/vector.hpp>

#include "intops.hpp"
#include "err.hpp"
#include "ox/std/hashmap.hpp"
#include "presenceindicator.hpp"
#include "types.hpp"

namespace ox {

class MetalClawWriter {

	private:
		FieldPresenceIndicator m_fieldPresence;
		int m_fields = 0;
		int m_field = 0;
		int m_unionIdx = -1;
		std::size_t m_buffIt = 0;
		std::size_t m_buffLen = 0;
		uint8_t *m_buff = nullptr;

	public:
		MetalClawWriter(uint8_t *buff, std::size_t buffLen, int unionIdx = -1) noexcept;

		~MetalClawWriter() noexcept;

		[[nodiscard]] Error field(const char*, int8_t *val) noexcept;
		[[nodiscard]] Error field(const char*, int16_t *val) noexcept;
		[[nodiscard]] Error field(const char*, int32_t *val) noexcept;
		[[nodiscard]] Error field(const char*, int64_t *val) noexcept;

		[[nodiscard]] Error field(const char*, uint8_t *val) noexcept;
		[[nodiscard]] Error field(const char*, uint16_t *val) noexcept;
		[[nodiscard]] Error field(const char*, uint32_t *val) noexcept;
		[[nodiscard]] Error field(const char*, uint64_t *val) noexcept;

		[[nodiscard]] Error field(const char*, bool *val) noexcept;

		template<typename T>
		[[nodiscard]] Error field(const char*, T *val, std::size_t len);

		template<typename T>
		[[nodiscard]] Error field(const char*, Vector<T> *val);

		template<typename T>
		[[nodiscard]] Error field(const char*, HashMap<String, T> *val);

		template<std::size_t L>
		[[nodiscard]] Error field(const char*, ox::BString<L> *val) noexcept;

		[[nodiscard]] Error field(const char*, SerStr val) noexcept;

		template<typename T>
		[[nodiscard]] Error field(const char*, T *val);

		template<typename U>
		[[nodiscard]] Error field(const char*, UnionView<U> val);

		template<typename T = std::nullptr_t>
		void setTypeInfo(const char *name = T::TypeName, int fields = T::Fields);

		std::size_t size() noexcept;

		static constexpr auto opType() {
			return OpType::Write;
		}

	private:
		template<typename I>
		[[nodiscard]] Error appendInteger(I val) noexcept;

};

template<std::size_t L>
Error MetalClawWriter::field(const char *name, ox::BString<L> *val) noexcept {
	return field(name, SerStr(val->data(), val->cap()));
}

template<typename T>
Error MetalClawWriter::field(const char*, T *val) {
	bool fieldSet = false;
	if (val && (m_unionIdx == -1 || m_unionIdx == m_field)) {
		MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
		oxReturnError(model(&writer, val));
		if (static_cast<std::size_t>(writer.m_fieldPresence.getMaxLen()) < writer.m_buffIt) {
			m_buffIt += writer.m_buffIt;
			fieldSet = true;
		}
	}
	oxReturnError(m_fieldPresence.set(m_field, fieldSet));
	m_field++;
	return OxError(0);
}

template<typename U>
Error MetalClawWriter::field(const char*, UnionView<U> val) {
	bool fieldSet = false;
	if (val.get() && (m_unionIdx == -1 || m_unionIdx == m_field)) {
		MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt, val.idx());
		oxReturnError(model(&writer, val.get()));
		if (static_cast<std::size_t>(writer.m_fieldPresence.getMaxLen()) < writer.m_buffIt) {
			m_buffIt += writer.m_buffIt;
			fieldSet = true;
		}
	}
	oxReturnError(m_fieldPresence.set(m_field, fieldSet));
	m_field++;
	return OxError(0);
}

template<typename T>
Error MetalClawWriter::field(const char*, T *val, std::size_t len) {
	bool fieldSet = false;

	if (len && (m_unionIdx == -1 || m_unionIdx == m_field)) {
		// write the length
		const auto arrLen = mc::encodeInteger(len);
		if (m_buffIt + arrLen.length < m_buffLen) {
			ox_memcpy(&m_buff[m_buffIt], arrLen.data, arrLen.length);
			m_buffIt += arrLen.length;
		} else {
			return OxError(MC_BUFFENDED);
		}

		MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
		writer.setTypeInfo<T>("List", len);

		// write the array
		for (std::size_t i = 0; i < len; i++) {
			oxReturnError(writer.field("", &val[i]));
		}

		m_buffIt += writer.m_buffIt;
		fieldSet = true;
	}

	oxReturnError(m_fieldPresence.set(m_field, fieldSet));
	m_field++;
	return OxError(0);
}

template<typename T>
Error MetalClawWriter::field(const char*, Vector<T> *val) {
	return field(nullptr, val->data(), val->size());
}

template<typename T>
[[nodiscard]] Error MetalClawWriter::field(const char*, HashMap<String, T> *val) {
	auto &keys = val->keys();
	auto len = keys.size();
	bool fieldSet = false;

	if (len && (m_unionIdx == -1 || m_unionIdx == m_field)) {
		// write the length
		const auto arrLen = mc::encodeInteger(len);
		if (m_buffIt + arrLen.length < m_buffLen) {
			ox_memcpy(&m_buff[m_buffIt], arrLen.data, arrLen.length);
			m_buffIt += arrLen.length;
		} else {
			return OxError(MC_BUFFENDED);
		}

		MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
		// double len for both key and value
		writer.setTypeInfo("Map", len * 2);

		// write the array
		for (std::size_t i = 0; i < len; i++) {
			auto &key = keys[i];
			const auto keyLen = ox_strlen(key);
			auto wkey = ox_malloca(keyLen + 1, char, 0);
			memcpy(wkey, key.c_str(), keyLen + 1);
			oxReturnError(writer.field("", SerStr(wkey.get(), keyLen)));
			oxReturnError(writer.field("", &(*val)[key]));
		}

		m_buffIt += writer.m_buffIt;
		fieldSet = true;
	}

	oxReturnError(m_fieldPresence.set(m_field, fieldSet));
	m_field++;
	return OxError(0);
}

template<typename I>
Error MetalClawWriter::appendInteger(I val) noexcept {
	bool fieldSet = false;
	if (val && (m_unionIdx == -1 || m_unionIdx == m_field)) {
		auto mi = mc::encodeInteger(val);
		if (mi.length < m_buffLen) {
			fieldSet = true;
			ox_memcpy(&m_buff[m_buffIt], mi.data, mi.length);
			m_buffIt += mi.length;
		} else {
			oxReturnError(OxError(MC_BUFFENDED));
		}
	}
	oxReturnError(m_fieldPresence.set(m_field, fieldSet));
	m_field++;
	return OxError(0);
;
}

template<typename T>
void MetalClawWriter::setTypeInfo(const char*, int fields) {
	m_fields = fields;
	m_fieldPresence.setFields(fields);
	m_buffIt = m_fieldPresence.getMaxLen();
	ox_memset(m_buff, 0, m_buffIt);
}

template<typename T>
ValErr<Vector<char>> writeMC(T *val) {
	Vector<char> buff(10 * units::MB);
	MetalClawWriter writer(bit_cast<uint8_t*>(buff.data()), buff.size());
	oxReturnError(model(&writer, val));
	buff.resize(writer.size());
	return buff;
}

template<typename T>
Error writeMC(uint8_t *buff, std::size_t buffLen, T *val, std::size_t *sizeOut = nullptr) {
	MetalClawWriter writer(buff, buffLen);
	auto err = model(&writer, val);
	if (sizeOut) {
		*sizeOut = writer.size();
	}
	return err;
}

}
