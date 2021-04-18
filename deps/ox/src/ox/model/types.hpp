/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/bstring.hpp>
#include <ox/std/strops.hpp>
#include <ox/std/types.hpp>
#include <ox/std/typetraits.hpp>

namespace ox {

class SerStr {

	protected:
		int m_cap = 0;
		char *m_str = nullptr;
		char **m_tgt = nullptr;

	public:
		template<std::size_t sz>
		constexpr SerStr(BString<sz> *str) noexcept {
			m_str = str->data();
			m_cap = str->cap();
		}

		constexpr SerStr(char *str, int cap) noexcept {
			m_str = str;
			m_cap = cap;
		}

		constexpr SerStr(char **tgt, int cap = -1) noexcept {
			m_tgt = tgt;
			m_str = const_cast<char*>(*tgt);
			m_cap = cap;
		}

		template<std::size_t cap>
		constexpr SerStr(char (&str)[cap]) noexcept {
			m_str = str;
			m_cap = cap;
		}

		constexpr const char *c_str() const noexcept {
			return m_str;
		}

		constexpr char *data(std::size_t sz = 0) noexcept {
			if (m_tgt && sz) {
				*m_tgt = new char[sz];
				m_str = *m_tgt;
				m_cap = static_cast<int>(sz);
			}
			return m_str;
		}

		constexpr int len() const noexcept {
			return static_cast<int>(m_str ? ox_strlen(m_str) : 0);
		}

		constexpr int cap() const noexcept {
			return m_cap;
		}

};

template<typename Union>
class UnionView {

	protected:
		int m_idx = -1;
		typename enable_if<is_union_v<Union>, Union>::type *m_union = nullptr;

	public:
		constexpr UnionView(Union *u, int idx) noexcept: m_idx(idx), m_union(u) {
		}

		constexpr auto idx() const noexcept {
			return m_idx;
		}

		constexpr const Union *get() const noexcept {
			return m_union;
		}

		constexpr Union *get() noexcept {
			return m_union;
		}

};

}
