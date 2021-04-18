/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>
#include <ox/std/types.hpp>

namespace ox {

template<typename T>
class FieldBitmapReader {
	protected:
		T m_map = nullptr;
		std::size_t m_mapLen = 0;
		std::size_t m_fields = 0;

	public:
		FieldBitmapReader(T map, std::size_t maxLen) noexcept;

		Result<bool> get(std::size_t i) const noexcept;

		void setFields(int) noexcept;

		void setMaxLen(int) noexcept;

		int getMaxLen() const noexcept;

};

template<typename T>
FieldBitmapReader<T>::FieldBitmapReader(T map, std::size_t maxLen) noexcept {
	m_map = map;
	m_mapLen = maxLen;
}

template<typename T>
Result<bool> FieldBitmapReader<T>::get(std::size_t i) const noexcept {
	if (i / 8 < m_mapLen) {
		return (m_map[i / 8] >> (i % 8)) & 1;
	} else {
		return OxError(MC_PRESENCEMASKOUTBOUNDS);
	}
}

template<typename T>
void FieldBitmapReader<T>::setFields(int fields) noexcept {
	m_fields = fields;
	m_mapLen = (fields / 8 + 1) - (fields % 8 == 0);
}

template<typename T>
void FieldBitmapReader<T>::setMaxLen(int maxLen) noexcept {
	m_mapLen = maxLen;
}

template<typename T>
int FieldBitmapReader<T>::getMaxLen() const noexcept {
	return m_mapLen;
}

extern template class FieldBitmapReader<uint8_t*>;
extern template class FieldBitmapReader<const uint8_t*>;

class FieldBitmap: public FieldBitmapReader<uint8_t*> {

	public:
		FieldBitmap(uint8_t *map, std::size_t maxLen) noexcept;

		Error set(std::size_t i, bool on) noexcept;

};

}
