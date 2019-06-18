/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <vector>
#include <nostalgia/core/gba/gba.hpp>
#include <nostalgia/common/point.hpp>
#include <ox/clargs/clargs.hpp>
#include <ox/fs/fs.hpp>
#include <ox/std/units.hpp>

#include "pack/pack.hpp"

using namespace std;
using namespace ox;
using namespace nostalgia::core;
using namespace nostalgia::common;

ox::ValErr<std::vector<uint8_t>> loadFileBuff(std::string path, ::size_t *sizeOut = nullptr) {
	auto file = fopen(path.c_str(), "rb");
	if (file) {
		fseek(file, 0, SEEK_END);
		const auto size = ftell(file);
		rewind(file);
		std::vector<uint8_t> buff(size);
		auto itemsRead = fread(buff.data(), buff.size(), 1, file);
		fclose(file);
		if (sizeOut) {
			*sizeOut = itemsRead ? size : 0;
		}
		return buff;
	} else {
		return {{}, OxError(1)};
	}
}

ox::Error run(ClArgs args) {
	std::string argSrc = args.getString("src").c_str();
	std::string argDst = args.getString("dst").c_str();
	if (argSrc == "") {
		std::cerr << "error: must specify a source directory\n";
		return OxError(1);
	}
	if (argDst == "") {
		std::cerr << "error: must specify a destination ROM file\n";
		return OxError(1);
	}
	std::vector<uint8_t> buff(32 * ox::units::MB);
	ox::FileSystem32::format(buff.data(), buff.size());
	ox::PassThroughFS src(argSrc.c_str());
	ox::FileSystem32 dst(ox::FileStore32(buff.data(), buff.size()));
	oxReturnError(nostalgia::pack(&src, &dst));
	return OxError(0);
}

int main(int argc, const char **args) {
	return static_cast<int>(run(ClArgs(argc, args)));
}
