/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <ox/std/trace.hpp>
#include <nostalgia/core/core.hpp>

#include "app.hpp"

int main(int argc, const char **argv) {
	if (argc > 1) {
		ox::trace::init();
		auto path = argv[1];
		auto [fs, err] = nostalgia::core::loadRomFs(path);
		if (err) {
			oxAssert(err, "Something went wrong...");
			return static_cast<int>(err);
		}
		err = run(fs);
		oxAssert(err, "Something went wrong...");
		delete fs;
		return static_cast<int>(err);
	}
	return 1;
}
