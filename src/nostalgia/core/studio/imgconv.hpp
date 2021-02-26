/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QString>

#include <ox/std/error.hpp>
#include <ox/std/types.hpp>

#include <ox/mc/mc.hpp>

#include <nostalgia/core/gfx.hpp>

namespace nostalgia::core {

[[nodiscard]] constexpr int pointToIdx(int w, int x, int y) noexcept {
	constexpr auto colLength = PixelsPerTile;
	const auto rowLength = (w / TileWidth) * colLength;
	const auto colStart = colLength * (x / TileWidth);
	const auto rowStart = rowLength * (y / TileHeight);
	const auto colOffset = x % TileWidth;
	const auto rowOffset = (y % TileHeight) * TileHeight;
	return colStart + colOffset + rowStart + rowOffset;
}

    [[nodiscard]] std::unique_ptr<core::NostalgiaGraphic> imgToNg(QString argInPath, int argBpp = -1);

}
