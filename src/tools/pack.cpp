/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <QColor>
#include <QImage>
#include <QMap>
#include <core/gba/gba.hpp>
#include <common/point.hpp>
#include <ox/clargs/clargs.hpp>
#include <ox/fs/filesystem.hpp>

using namespace std;
using namespace ox;
using namespace ox::clargs;
using namespace ox::fs;
using namespace nostalgia::core;
using namespace nostalgia::common;

uint8_t *loadFileBuff(QString path, ::size_t *sizeOut = nullptr) {
	auto file = fopen(path.toUtf8(), "rb");
	if (file) {
		fseek(file, 0, SEEK_END);
		const auto size = ftell(file);
		rewind(file);
		auto buff = new uint8_t[size];
		auto itemsRead = fread(buff, size, 1, file);
		fclose(file);
		if (sizeOut) {
			*sizeOut = itemsRead ? size : 0;
		}
		return buff;
	} else {
		return nullptr;
	}
}

uint16_t toGbaColor(QColor c) {
	auto r = ((uint32_t) c.red()) >> 3;
	auto g = ((uint32_t) c.green()) >> 3;
	auto b = ((uint32_t) c.blue()) >> 3;
	return (r << 10) | (g << 5) | (b << 0);
}

int pointToIdx(int w, int x, int y) {
	const auto colLength = 64;
	const auto rowLength = (w / 8) * colLength;
	const auto colStart = colLength * (x / 8);
	const auto rowStart = rowLength * (y / 8);
	const auto colOffset = x % 8;
	const auto rowOffset = (y % 8) * 8;
	return colStart + colOffset + rowStart + rowOffset;
}

int run(ClArgs args) {
	Error err = 0;
	QString inPath = args.getString("img").c_str();
	QString fsPath = args.getString("fs").c_str();
	auto compact = args.getBool("c");
	QMap<QRgb, int> colors;
	QImage src(inPath);
	const auto imgDataBuffSize = sizeof(Pallete) - 1 + src.width() * src.height();
	uint8_t imgDataBuff[imgDataBuffSize];
	GbaImageData *id = (GbaImageData*) imgDataBuff;
	int colorId = 0;

	if (!src.isNull()) {

		// copy pixels as color ids
		for (int x = 0; x < src.colorCount(); x++) {
			for (int y = 0; y < src.colorCount(); y++) {
				auto destI = pointToIdx(src.width(), x, y);
				auto c = src.pixel(x, y);
				if (!colors.contains(c)) {
					colors[c] = colorId;
					colorId++;
				}
				((uint8_t*) &id->tiles)[destI] = colors[c];
			}
		}

		// store colors in palette with the corresponding color id
		for (auto key : colors.keys()) {
			auto colorId = colors[key];
			id->pal[colorId] = toGbaColor(key);
		}


		size_t fsBuffSize;
		auto fsBuff = loadFileBuff(fsPath, &fsBuffSize);
		if (fsBuff && !err) {
			auto fs = createFileSystem(fsBuff, fsBuffSize);

			if (fs) {
				fs = expandCopyCleanup(fs, fs->size() + fs->spaceNeeded(imgDataBuffSize));
				fsBuff = fs->buff(); // update fsBuff pointer in case there is a new buff
				err |= fs->write(1, imgDataBuff, imgDataBuffSize);

				if (compact) {
					fs->resize();
				}

				auto fsFile = fopen(fsPath.toUtf8(), "wb");
				if (fsFile) {
					err = fwrite(fsBuff, fs->size(), 1, fsFile) != 1;
					err |= fclose(fsFile);
					if (err) {
						cerr << "Could not write to file system file.\n";
					}
				} else {
					err = 2;
				}

				delete fs;
			} else {
				err = 3;
			}
		}

		delete[] fsBuff;
	} else {
		err = 4;
	}

	return err;
}

int main(int argc, const char **args) {
	return run(ClArgs(argc, args));
}
