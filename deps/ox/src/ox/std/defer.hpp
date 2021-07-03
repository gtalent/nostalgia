/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "error.hpp"

namespace ox {

template<typename T>
class Defer {
	private:
		T m_deferredFunc;

	public:
		Defer(T deferredFunc) {
			m_deferredFunc = deferredFunc;
		}

		Defer(const Defer&) = delete;

		~Defer() {
			m_deferredFunc();
		}

		Defer &operator=(const Defer&) = delete;

};

}

#define oxDefer Defer oxConcat(oxDefer_, __LINE__) = [&]