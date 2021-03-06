/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/hashmap.hpp>
#include <ox/std/string.hpp>

namespace ox {

class ClArgs {
	private:
		HashMap<String, bool> m_bools;
		HashMap<String, String> m_strings;
		HashMap<String, int> m_ints;

	public:
		ClArgs(int argc, const char **args) noexcept;

		bool getBool(const char *arg, bool defaultValue) const noexcept;

		String getString(const char *argName, const char *defaultArg) const noexcept;

		int getInt(const char *arg, int defaultValue) const noexcept;

		Result<bool> getBool(const char *arg) const noexcept;

		Result<String> getString(const char *argName) const noexcept;

		Result<int> getInt(const char *arg) const noexcept;

};

}
