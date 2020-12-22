/*
 * Copyright 2015 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <map>
#include <string>

namespace ox {

class ClArgs {
	private:
		::std::map<::std::string, bool> m_bools;
		::std::map<::std::string, ::std::string> m_strings;
		::std::map<::std::string, int> m_ints;

	public:
		ClArgs(int argc, const char **args);

		bool getBool(const char *arg);

		::std::string getString(const char *argName, const char *defaultArg = "");

		int getInt(const char *arg);
};

}
