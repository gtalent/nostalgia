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

		Error field(const char*, int8_t *val);
		Error field(const char*, int16_t *val);
		Error field(const char*, int32_t *val);
		Error field(const char*, int64_t *val);

		Error field(const char*, uint8_t *val);
		Error field(const char*, uint16_t *val);
		Error field(const char*, uint32_t *val);
		Error field(const char*, uint64_t *val);

		Error field(const char*, bool *val);

		template<typename T>
		Error field(const char*, T *val, std::size_t len);

		template<typename T>
		Error field(const char*, ox::Vector<T> *val);

		template<std::size_t L>
		Error field(const char*, ox::BString<L> *val);

		Error field(const char*, SerStr val);

		template<typename T>
		Error field(const char*, T *val);

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
Error MetalClawWriter::field(const char *name, ox::BString<L> *val) {
	return field(name, SerStr(val->data(), val->cap()));
}

template<typename T>
Error MetalClawWriter::field(const char*, T *val) {
	bool fieldSet = false;
	MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
	if (val) {
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

template<typename T>
Error MetalClawWriter::field(const char*, ox::Vector<T> *val) {
	return field(nullptr, val->data(), val->size());
}

template<typename T>
Error MetalClawWriter::field(const char*, T *val, std::size_t len) {
	auto err = OxError(0);
	bool fieldSet = false;

	if (len) {
		// write the length
		const auto arrLen = mc::encodeInteger(len);
		if (m_buffIt + arrLen.length < m_buffLen) {
			ox_memcpy(&m_buff[m_buffIt], arrLen.data, arrLen.length);
			m_buffIt += arrLen.length;
		} else {
			err = OxError(MC_BUFFENDED);
		}

		MetalClawWriter writer(m_buff + m_buffIt, m_buffLen - m_buffIt);
		writer.setTypeInfo("List", len);

		// write the array
		for (std::size_t i = 0; i < len; i++) {
			err |= writer.field("", &val[i]);
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
	auto err = OxError(0);
	bool fieldSet = false;
	if (val) {
		auto mi = mc::encodeInteger(val);
		if (mi.length < m_buffLen) {
			fieldSet = true;
			ox_memcpy(&m_buff[m_buffIt], mi.data, mi.length);
			m_buffIt += mi.length;
		} else {
			err |= OxError(MC_BUFFENDED);
		}
	}
	err |= m_fieldPresence.set(m_field, fieldSet);
	m_field++;
	return err;
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
