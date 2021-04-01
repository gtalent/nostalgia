/*
 * Copyright 2015 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/string.hpp>
#include "clargs.hpp"

namespace ox {

ClArgs::ClArgs(int argc, const char **args) {
	for (int i = 0; i < argc; i++) {
		String arg = args[i];
		if (arg[0] == '-') {
			while (arg[0] == '-' && arg.len()) {
				arg = arg.c_str() + 1;
			}
			m_bools[arg] = true;

			// parse additional arguments
			if (i < argc && args[i + 1]) {
				String val = args[i + 1];
				if (val.len() && val[i] != '-') {
					if (val == "false") {
						m_bools[arg] = false;
					}
					m_strings[arg] = val;
					if (auto r = ox_atoi(val.c_str()); r.error == 0) {
						m_ints[arg] = r.value;
					}
					i++;
				}
			}
		}
	}
}

bool ClArgs::getBool(const char *arg) const {
	return m_bools[arg];
}

String ClArgs::getString(const char *argName, const char *defaultArg) const {
	return m_strings.contains(argName) ? m_strings[argName].c_str() : defaultArg;
}

int ClArgs::getInt(const char *arg) const {
	return m_ints[arg];
}

}
