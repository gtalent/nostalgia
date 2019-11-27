/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QColor>
#include <QImage>
#include <QMap>

#include <nostalgia/core/core.hpp>

#include "imgconv.hpp"

namespace nostalgia {

namespace {

[[nodiscard]] uint16_t toGbaColor(QColor c) {
	const auto r = static_cast<uint32_t>(c.red()) >> 3;
	const auto g = static_cast<uint32_t>(c.green()) >> 3;
	const auto b = static_cast<uint32_t>(c.blue()) >> 3;
	return (r << 10) | (g << 5) | (b << 0);
}

[[nodiscard]] int pointToIdx(int w, int x, int y) {
	constexpr auto colLength = 64;
	const auto rowLength = (w / 8) * colLength;
	const auto colStart = colLength * (x / 8);
	const auto rowStart = rowLength * (y / 8);
	const auto colOffset = x % 8;
	const auto rowOffset = (y % 8) * 8;
	return colStart + colOffset + rowStart + rowOffset;
}

}

[[nodiscard]] static int countColors(const QImage &img, int argTiles) {
	QMap<QRgb, bool> colors;
	// copy pixels as color ids
	for (int x = 0; x < img.width(); x++) {
		for (int y = 0; y < img.height(); y++) {
			auto destI = pointToIdx(img.width(), x, y);
			if (destI <= argTiles * 64) {
				auto c = img.pixel(x, y);
				// assign color a color id for the palette
				if (!colors.contains(c)) {
					colors[c] = true;
				}
			}
		}
	}
	return colors.size();
}

[[nodiscard]] std::unique_ptr<core::NostalgiaGraphic> imgToNg(QString argSrc, int argTiles, int argBpp) {
	constexpr auto TilePixels = 64;
	QImage src(argSrc);

	if (src.isNull()) {
		return {};
	}

	const auto Pixels = argTiles ? argTiles * TilePixels : src.width() * src.height();
	if (argTiles == 0) {
		argTiles = Pixels / 64;
	}
	const auto Colors = countColors(src, argTiles);
	if (argBpp != 4 && argBpp != 8) {
		argBpp = Colors > 16 ? 8 : 4;
	}

	QMap<QRgb, int> colors;
	auto ng = std::make_unique<core::NostalgiaGraphic>();
	ng->pal.colors.resize(countColors(src, argTiles));
	if (argBpp == 4) {
		ng->tiles.resize(Pixels / 2);
	} else {
		ng->tiles.resize(Pixels);
	}
	ng->bpp = argBpp;

	int colorIdx = 0;
	// copy pixels as color ids
	for (int x = 0; x < src.width(); x++) {
		for (int y = 0; y < src.height(); y++) {
			auto destI = pointToIdx(src.width(), x, y);
			if (destI < argTiles * 64) {
				const auto c = src.pixel(x, y);
				// assign color a color id for the palette
				if (!colors.contains(c)) {
					colors[c] = colorIdx;
					colorIdx++;
				}
				// set pixel color
				if (argBpp == 4) {
					if (destI % 2) { // is odd number pixel
						ng->tiles[destI / 2] |= colors[c] << 4;
					} else {
						ng->tiles[destI / 2] |= colors[c];
					}
				} else {
					ng->tiles[destI] = colors[c];
				}
			}
		}
	}

	// store colors in palette with the corresponding color id
	for (auto key : colors.keys()) {
		auto colorId = colors[key];
		ng->pal.colors[colorId] = toGbaColor(key);
	}

	return ng;
}

}
