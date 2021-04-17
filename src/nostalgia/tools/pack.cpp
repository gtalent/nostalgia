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
#include <ox/std/trace.hpp>

#include "pack/pack.hpp"

static ox::Error writeFileBuff(const std::string_view path, ox::Vector<char> &buff) {
	try {
		std::ofstream f(path, std::ios::binary);
		f.write(buff.data(), static_cast<intptr_t>(buff.size()));
	} catch (const std::fstream::failure&) {
		return OxError(2, "failed to write file");
	}
	return OxError(0);
}

static ox::Error run(const ox::ClArgs &args) {
	auto argSrc = args.getString("src", "");
	auto argDst = args.getString("dst", "");
	if (argSrc == "") {
		std::cerr << "error: must specify a source directory\n";
		return OxError(1, "must specify a source directory");
	}
	if (argDst == "") {
		std::cerr << "error: must specify a destination ROM file\n";
		return OxError(1, "must specify a destination ROM file");
	}
	ox::Vector<char> buff(32 * ox::units::MB);
	oxReturnError(ox::FileSystem32::format(buff.data(), buff.size()));
	ox::PassThroughFS src(argSrc.c_str());
	ox::FileSystem32 dst(ox::FileStore32(buff.data(), buff.size()));
	oxReturnError(nostalgia::pack(&src, &dst));

	oxReturnError(dst.resize());
	oxRequire(dstSize, dst.size());
	std::cout << "new size: " << dstSize << '\n';
	buff.resize(dstSize);

	oxReturnError(writeFileBuff(argDst.c_str(), buff));
	return OxError(0);
}

int main(int argc, const char **args) {
	ox::trace::init();
	auto err = run(ox::ClArgs(argc, args));
	oxAssert(err, "pack failed");
	return static_cast<int>(err);
}
