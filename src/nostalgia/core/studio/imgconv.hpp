/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QString>

#include <ox/std/error.hpp>
#include <ox/std/types.hpp>

#include <ox/mc/mc.hpp>

#include <nostalgia/core/gfx.hpp>

namespace nostalgia {

template<typename T>
[[nodiscard]] ox::ValErr<std::vector<uint8_t>> toBuffer(T *data, std::size_t buffSize = ox::units::MB) {
	std::vector<uint8_t> buff(buffSize);
	std::size_t sz = 0;
	oxReturnError(ox::writeMC(buff.data(), buff.size(), data, &sz));
	if (sz > buffSize) {
		return OxError(1);
	}
	buff.resize(sz);
	return buff;
}

[[nodiscard]] std::unique_ptr<core::NostalgiaGraphic> imgToNg(QString argInPath, int argTiles, int argBpp = -1);

}
