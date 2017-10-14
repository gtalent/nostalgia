/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "world.hpp"

namespace nostalgia {
namespace world {

Zone::Zone() {
	m_bounds.x = -1;
	m_bounds.y = -1;
	m_bounds.width = -1;
	m_bounds.height = -1;
}

void Zone::draw(core::Context *ctx) {
}

size_t Zone::size() {
	return sizeof(Zone) + m_bounds.width * m_bounds.height * sizeof(TileDef);
}

TileDef *Zone::tiles() {
	return (TileDef*) (this + 1);
}

TileDef *Zone::tile(int row, int column) {
	return &tiles()[row * m_bounds.width + column];
}

void Zone::setTile(int row, int column, TileDef *td) {
	tiles()[row * m_bounds.width + column] = *td;
}

}
}
