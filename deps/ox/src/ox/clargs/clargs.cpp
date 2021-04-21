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

ClArgs::ClArgs(int argc, const char **args) noexcept {
	for (int i = 0; i < argc; i++) {
		String arg = args[i];
		if (arg[0] == '-') {
			while (arg[0] == '-' && arg.len()) {
				arg = arg.substr(1);
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

bool ClArgs::getBool(const char *arg, bool defaultValue) const noexcept {
	auto [value, err] = m_ints.at(arg);
	return !err ? value : defaultValue;
}

String ClArgs::getString(const char *arg, const char *defaultValue) const noexcept {
	auto [value, err] = m_strings.at(arg);
	return !err ? value : defaultValue;
}

int ClArgs::getInt(const char *arg, int defaultValue) const noexcept {
	auto [value, err] = m_ints.at(arg);
	return !err ? value : defaultValue;
}

Result<bool> ClArgs::getBool(const char *arg) const noexcept {
	oxRequire(out, m_bools.at(arg));
	return out;
}

Result<String> ClArgs::getString(const char *argName) const noexcept {
	oxRequire(out, m_strings.at(argName));
	return out;
}

Result<int> ClArgs::getInt(const char *arg) const noexcept {
	oxRequire(out, m_ints.at(arg));
	return out;
}

}
