/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/fs.hpp>

namespace nostalgia::core {

// User Input Output
class Context {
	public:
		ox::FileSystem *rom = nullptr;
	private:
		void *m_implData = nullptr;

	public:
		constexpr void setImplData(void *implData) noexcept {
			m_implData = implData;
		}

		template<typename T>
		constexpr T *implData() noexcept {
			return static_cast<T*>(m_implData);
		}

};

}

