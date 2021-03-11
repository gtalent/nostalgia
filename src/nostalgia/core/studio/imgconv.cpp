/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
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

namespace nostalgia::core {

namespace {

[[nodiscard]] uint16_t toGbaColor(QColor c) {
	const auto r = static_cast<uint32_t>(c.red()) >> 3;
	const auto g = static_cast<uint32_t>(c.green()) >> 3;
	const auto b = static_cast<uint32_t>(c.blue()) >> 3;
	const auto a = static_cast<uint32_t>(c.alpha() > 128 ? 1 : 0);
	return (a << 15) | (r << 10) | (g << 5) | (b << 0);
}

}

[[nodiscard]] static int countColors(const QImage &img, int argTiles) {
	QMap<QRgb, bool> colors;
	// copy pixels as color ids
	for (int x = 0; x < img.width(); x++) {
		for (int y = 0; y < img.height(); y++) {
			auto destI = pointToIdx(img.width(), x, y);
			if (destI <= argTiles * PixelsPerTile) {
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

[[nodiscard]] std::unique_ptr<core::NostalgiaGraphic> imgToNg(QString argSrc, int argBpp) {
	QImage src(argSrc);

	if (src.isNull()) {
		return {};
	}

	const auto Pixels = src.width() * src.height();
	const auto Tiles = Pixels / PixelsPerTile;
	const auto Colors = countColors(src, Tiles);
	if (argBpp != 4 && argBpp != 8) {
		argBpp = Colors > 16 ? 8 : 4;
	}

	QMap<QRgb, int> colors;
	auto ng = std::make_unique<core::NostalgiaGraphic>();
	ng->pal.colors.resize(static_cast<std::size_t>(countColors(src, Tiles)));
	if (argBpp == 4) {
		ng->pixels.resize(static_cast<std::size_t>(Pixels / 2));
	} else {
		ng->pixels.resize(static_cast<std::size_t>(Pixels));
	}
	ng->bpp = argBpp;
	ng->columns = src.width() / TileWidth;
	ng->rows = src.height() / TileHeight;

	int colorIdx = 0;
	// copy pixels as color ids
	for (int x = 0; x < src.width(); x++) {
		for (int y = 0; y < src.height(); y++) {
			auto destI = pointToIdx(src.width(), x, y);
			if (destI < Tiles * PixelsPerTile) {
				const auto c = src.pixel(x, y);
				// assign color a color id for the palette
				if (!colors.contains(c)) {
					colors[c] = colorIdx;
					colorIdx++;
				}
				// set pixel color
				if (argBpp == 4) {
					if (destI % 2) { // is odd number pixel
						ng->pixels[static_cast<std::size_t>(destI / 2)] |= colors[c] << 4;
					} else {
						ng->pixels[static_cast<std::size_t>(destI / 2)] |= colors[c];
					}
				} else {
					ng->pixels[static_cast<std::size_t>(destI)] = static_cast<std::size_t>(colors[c]);
				}
			}
		}
	}

	// store colors in palette with the corresponding color id
	for (auto key : colors.keys()) {
		auto colorId = static_cast<std::size_t>(colors[key]);
		ng->pal.colors[colorId] = toGbaColor(key);
	}

	return ng;
}

}
