/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>
#include <iostream>
#include <QColor>
#include <QImage>
#include <QMap>
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

ox::ValErr<std::vector<uint8_t>> loadFileBuff(QString path, ::size_t *sizeOut = nullptr) {
	auto file = fopen(path.toUtf8(), "rb");
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

int run(ClArgs args) {
	QString argSrc = args.getString("src").c_str();
	QString argDst = args.getString("dst").c_str();
	std::array<uint8_t, 32 * ox::units::MB> buff;
	ox::FileSystem32::format(buff.data(), buff.size());
	ox::PassThroughFS src(argSrc.toUtf8());
	ox::FileSystem32 dst(ox::FileStore32(buff.data(), buff.size()));
	oxReturnError(nostalgia::pack(&src, &dst));
	return 0;
}

int main(int argc, const char **args) {
	return run(ClArgs(argc, args));
}
