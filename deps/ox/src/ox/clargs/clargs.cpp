/*
 * Copyright 2015 - 2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/strops.hpp>
#include "clargs.hpp"

namespace ox {

using namespace ::std;

ClArgs::ClArgs(int argc, const char **args) {
	for (int i = 0; i < argc; i++) {
		string arg = args[i];
		if (arg[0] == '-') {
			while (arg[0] == '-' && arg.size()) {
				arg = arg.substr(1);
			}
			m_bools[arg] = true;

			// parse additional arguments
			if (i < argc && args[i + 1]) {
				string val = args[i + 1];
				if (val.size() && val[i] != '-') {
					if (val == "false") {
						m_bools[arg] = false;
					}
					m_strings[arg] = val;
					m_ints[arg] = ox_atoi(val.c_str());
					i++;
				}
			}
		}
	}
}

bool ClArgs::getBool(const char *arg) {
	return m_bools[arg];
}

string ClArgs::getString(const char *arg) {
	return m_strings[arg];
}

int ClArgs::getInt(const char *arg) {
	return m_ints[arg];
}

}
