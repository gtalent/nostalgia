/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

namespace nostalgia::core {

constexpr auto PluginName = "NostalgiaCore";
constexpr auto TileSheetDir = "/TileSheets/";
constexpr auto PaletteDir = "/Palettes/";

// Command IDs to use with QUndoCommand::id()
enum class CommandId {
	UpdatePixel = 1,
	ModPixel = 2,
	UpdateDimension = 3,
	InsertTile = 4,
};

}
