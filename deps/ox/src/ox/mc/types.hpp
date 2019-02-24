/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/strops.hpp>
#include <ox/std/types.hpp>

namespace ox {

using StringLength = uint32_t;
using ArrayLength = uint32_t;

class McStr {

	protected:
		int m_cap = -1;
		char *m_str = nullptr;

	public:
		explicit constexpr McStr(const char *str) noexcept {
			m_str = const_cast<char*>(str);
			m_cap = -1;
		}

		constexpr McStr(char *str, int cap) noexcept {
			m_str = str;
			m_cap = cap;
		}

		constexpr const char *c_str() noexcept {
			return m_str;
		}

		constexpr char *data() noexcept {
			// do not return a non-const pointer to the const_casted m_str
			if (m_cap > -1) {
				return m_str;
			} else {
				return nullptr;
			}
		}

		constexpr int len() noexcept {
			return ox_strlen(m_str);
		}

		constexpr int bytes() noexcept {
			return ox_strlen(m_str) + 1; // adds 1 for \0
		}

		constexpr int cap() noexcept {
			return m_cap;
		}

};

}
