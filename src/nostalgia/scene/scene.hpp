/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/error.hpp>
#include <ox/std/types.hpp>
#include <ox/std/vector.hpp>

namespace nostalgia::scene {

struct TileDoc {

	constexpr static auto Fields = 2;
	constexpr static auto Preloadable = true;
	constexpr static auto TypeName = "net.drinkingtea.nostalgia.scene.Tile";
	constexpr static auto TypeVersion = 1;

	uint16_t sheetIdx = 0;
	uint8_t  type = 0;

};

template<typename T>
constexpr ox::Error model(T *io, TileDoc *obj) {
	io->template setTypeInfo<TileDoc>();
	oxReturnError(io->field("sheetIdx", &obj->sheetIdx));
	oxReturnError(io->field("type", &obj->type));
	return OxError(0);
}

struct SceneDoc {

	using TileMapRow = ox::Vector<TileDoc>;
	using TileMapLayer = ox::Vector<TileMapRow>;
	using TileMap = ox::Vector<TileMapLayer>;

	constexpr static auto Fields = 1;
	constexpr static auto Preloadable = true;
	constexpr static auto TypeName = "net.drinkingtea.nostalgia.scene.Scene";
	constexpr static auto TypeVersion = 1;

	TileMap tiles;

};

template<typename T>
constexpr ox::Error model(T *io, SceneDoc *obj) {
	io->template setTypeInfo<SceneDoc>();
	oxReturnError(io->field("tiles", &obj->tiles));
	return OxError(0);
}

struct SceneInstance {

	uint16_t layers = 0;
	uint16_t *columns = nullptr;
	uint16_t *rows = nullptr;
	uint16_t **tileMapIdx = nullptr;
	uint8_t  **tileType = nullptr;

};

}
