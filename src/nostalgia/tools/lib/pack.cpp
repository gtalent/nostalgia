/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QDebug>
#include <QDir>

#include <ox/fs/fs.hpp>

#include "tilesetimport.hpp"

namespace nostalgia {

ox::Error pack(QDir src, ox::FileSystem *dest) {
	for (auto entry : src.entryList()) {
		qDebug() << entry;
		oxReturnError(importTileSet(dest, entry, entry));
	}
	return OxError(0);
}

}

