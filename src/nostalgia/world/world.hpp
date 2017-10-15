/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/std/types.hpp>

#include <nostalgia/common/common.hpp>
#include <nostalgia/core/core.hpp>

namespace nostalgia {
namespace world {

struct TileDef {
	uint16_t bgTile;
};

template<typename T>
ox::Error ioOp(T *io, TileDef *obj) {
	ox::Error err = 0;
	io->setFields(1);
	err |= io->op("bgTile", &obj->bgTile);
	return err;
}


struct RegionDef {
	uint32_t tileSheetInodes[20];
};

struct ZoneDef {
	int32_t width = 0;
	int32_t height = 0;
};

class Zone {

	private:
		common::Bounds m_bounds;
		TileDef *m_tiles = nullptr;

	public:

		Zone(common::Bounds bnds);

		void draw(core::Context *ctx);

		size_t size();

		TileDef *tile(int row, int column);

		void setTile(int row, int column, TileDef *td);

};

class Region {

	private:
		Zone *m_zones;

	public:

};

}
}
