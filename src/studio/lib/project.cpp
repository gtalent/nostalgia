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

Project::Project(QString path) {
	m_path = path;
}

void Project::create() {
	QDir().mkpath(m_path);

	m_romBuff = new QByteArray(1024, 0);
	FileSystem32::format(m_romBuff->data(), m_romBuff->size(), true);
	auto fs = ox::fs::createFileSystem(m_romBuff->data(), m_romBuff->size());

	fs->mkdir("/Tilesets");

	QFile file(m_path + "/ROM.oxfs");
	file.open(QIODevice::WriteOnly);
	file.write(*m_romBuff);
	file.close();

	delete fs;
}

}
}
