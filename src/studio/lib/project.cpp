/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QByteArray>
#include <QDir>
#include <ox/fs/filesystem.hpp>
#include <project.hpp>

namespace nostalgia {
namespace studio {

using namespace ox::fs;

void Project::create(QString path) {
	QDir().mkpath(path);

	size_t buffLen = 1024;
	m_romBuff = new QByteArray(buffLen, 0);
	FileSystem32::format(m_romBuff->data(), buffLen, true);

	QFile file(path + "/ROM.oxfs");
	file.open(QIODevice::WriteOnly);
	file.write(*m_romBuff);
	file.close();
}

}
}
