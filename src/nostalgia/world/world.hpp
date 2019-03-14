/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/mc/mc.hpp>
#include <ox/std/std.hpp>

#include <nostalgia/common/common.hpp>
#include <nostalgia/core/core.hpp>

namespace nostalgia::world {

struct Tile {
	static constexpr auto Fields = 3;

	uint8_t bgTile = 0;
	uint8_t type = 0;
	void *occupant = nullptr;
};

template<typename T>
ox::Error ioOpRead(T *io, Tile *obj) {
	ox::Error err = 0;
	io->setTypeInfo("nostalgia::world::Tile", Tile::Fields);
	err |= io->op("bgTile", &obj->bgTile);
	err |= io->op("type", &obj->type);
	return err;
}


struct Zone {

	template<typename T>
	friend ox::Error ioOpRead(T*, Zone*);

	template<typename T>
	friend ox::Error ioOpWrite(T*, Zone*);

	protected:
		static constexpr auto Fields = 2;
		common::Bounds m_bounds;
		Tile *m_tiles = nullptr;

	public:
		Zone(core::Context *ctx, common::Bounds bnds, core::InodeId_t tileSheet);

		void draw(core::Context *ctx);

		std::size_t size();

		Tile *tile(int x, int y);

		void setTile(int x, int y, Tile *td);

};

template<typename T>
ox::Error ioOpRead(T *io, Zone *obj) {
	ox::Error err = 0;
	io->setTypeInfo("nostalgia::world::Zone", Zone::Fields);
	err |= io->op("bounds", &obj->m_bounds);
	return err;
}

template<typename T>
ox::Error ioOpWrite(T *io, Zone *obj) {
	ox::Error err = 0;
	io->setTypeInfo("nostalgia::world::Zone", Zone::Fields);
	err |= io->op("bounds", &obj->m_bounds);
	return err;
}


struct Region {

	template<typename T>
	friend ox::Error ioOpRead(T*, Region*);

	template<typename T>
	friend ox::Error ioOpWrite(T*, Region*);

	protected:
		static constexpr auto Fields = 1;
		ox::Vector<Zone*> m_zones;

	public:

};

template<typename T>
ox::Error ioOpRead(T *io, Region *obj) {
	ox::Error err = 0;
	io->setTypeInfo("nostalgia::World::Region", Region::Fields);
	return err;
}

template<typename T>
ox::Error ioOpWrite(T *io, Region *obj) {
	ox::Error err = 0;
	io->setTypeInfo("nostalgia::World::Region", Region::Fields);
	return err;
}

}
