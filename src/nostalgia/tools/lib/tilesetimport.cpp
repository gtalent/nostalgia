/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QColor>
#include <QImage>
#include <QMap>
#include <QVector>

#include <ox/clargs/clargs.hpp>
#include <ox/fs/fs.hpp>

#include <nostalgia/core/gba/gba.hpp>
#include <nostalgia/common/point.hpp>

namespace nostalgia {

using namespace nostalgia::core;
using namespace nostalgia::common;

[[nodiscard]] uint16_t toGbaColor(QColor c) {
	const auto r = static_cast<uint32_t>(c.red()) >> 3;
	const auto g = static_cast<uint32_t>(c.green()) >> 3;
	const auto b = static_cast<uint32_t>(c.blue()) >> 3;
	return (r << 10) | (g << 5) | (b << 0);
}

[[nodiscard]] constexpr int pointToIdx(int w, int x, int y) noexcept {
	constexpr auto colLength = 64;
	const auto rowLength = (w / 8) * colLength;
	const auto colStart = colLength * (x / 8);
	const auto rowStart = rowLength * (y / 8);
	const auto colOffset = x % 8;
	const auto rowOffset = (y % 8) * 8;
	return colStart + colOffset + rowStart + rowOffset;
}

[[nodiscard]] ox::ValErr<int> countColors(QString importPath) {
	QImage src(importPath);
	if (!src.isNull()) {
		return {{}, OxError(1)};
	}

	QMap<QRgb, bool> colors;

	// copy pixels as color ids
	for (int x = 0; x < src.width(); x++) {
		for (int y = 0; y < src.height(); y++) {
			auto c = src.pixel(x, y);
			// assign color a color id for the palette
			if (!colors.contains(c)) {
				colors[c] = true;
			}
		}
	}

	return colors.size();
}

[[nodiscard]] ox::ValErr<QVector<uint8_t>> convertImg(QString importPath, int bpp) {
	QImage src(importPath);
	if (!src.isNull()) {
		return {{}, OxError(1)};
	}

	QMap<QRgb, int> colors;
	auto tileCount = (src.width() * src.height()) / 64;
	const auto imgDataBuffSize = sizeof(GbaImageData) + 1 + tileCount * 64;
	QVector<uint8_t> imgDataBuff(imgDataBuffSize);
	memset(imgDataBuff.data(), 0, imgDataBuffSize);
	GbaImageData *id = reinterpret_cast<GbaImageData*>(imgDataBuff.data());
	id->header.bpp = bpp;
	id->header.tileCount = tileCount;
	int colorId = 0;

	// copy pixels as color ids
	for (int x = 0; x < src.width(); x++) {
		for (int y = 0; y < src.height(); y++) {
			auto destI = pointToIdx(src.width(), x, y);
			auto c = src.pixel(x, y);
			// assign color a color id for the palette
			if (!colors.contains(c)) {
				colors[c] = colorId;
				colorId++;
			}
			// set pixel color
			if (bpp == 4) {
				if (destI % 2) { // is odd number pixel
					id->tiles[destI / 2] |= colors[c] << 4;
				} else {
					id->tiles[destI / 2] |= colors[c];
				}
			} else {
				id->tiles[destI] = colors[c];
			}
		}
	}

	// store colors in palette with the corresponding color id
	for (auto key : colors.keys()) {
		auto colorId = colors[key];
		id->pal[colorId] = toGbaColor(key);
	}

	return imgDataBuff;
}

ox::Error importTileSet(ox::FileSystem *fs, QString romPath, QString importPath) {
	const auto bpp = countColors(importPath) > 16 ? 8 : 4;
	auto [imgDataBuff, err] = convertImg(importPath, bpp);
	oxReturnError(err);
	return fs->write(romPath.toUtf8().data(), imgDataBuff.data(), imgDataBuff.size());
}

}
