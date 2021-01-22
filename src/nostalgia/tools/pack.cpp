/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <ox/clargs/clargs.hpp>
#include <ox/fs/fs.hpp>
#include <ox/std/units.hpp>

#include "pack/pack.hpp"

static void writeFileBuff(const std::string &path, std::vector<char> &buff) {
	try {
		std::ofstream f(path, std::ios::binary);
		f.write(buff.data(), static_cast<intptr_t>(buff.size()));
	} catch (const std::fstream::failure&) {
		throw OxError(2);
	}
}

void run(ox::ClArgs args) {
	std::string argSrc = args.getString("src").c_str();
	std::string argDst = args.getString("dst").c_str();
	if (argSrc == "") {
		std::cerr << "error: must specify a source directory\n";
		throw OxError(1);
	}
	if (argDst == "") {
		std::cerr << "error: must specify a destination ROM file\n";
		throw OxError(1);
	}
	std::vector<char> buff(32 * ox::units::MB);
	oxThrowError(ox::FileSystem32::format(buff.data(), buff.size()));
	ox::PassThroughFS src(argSrc.c_str());
	ox::FileSystem32 dst(ox::FileStore32(buff.data(), buff.size()));
	oxThrowError(nostalgia::pack(&src, &dst));

	oxThrowError(dst.resize());
	std::cout << "new size: " << dst.size() << '\n';
	buff.resize(dst.size());

	writeFileBuff(argDst, buff);
}

int main(int argc, const char **args) {
	try {
		run(ox::ClArgs(argc, args));
	} catch (const ox::Error &err) {
		oxPanic(err, "pack failed");
		std::cerr << "pack failed...\n";
		return static_cast<int>(err);
	}
	return 0;
}
