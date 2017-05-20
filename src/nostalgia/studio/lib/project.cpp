/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QDir>

#include "project.hpp"

namespace nostalgia {
namespace studio {

using namespace ox;

QString Project::ROM_FILE = "/ROM.oxfs";

Project::Project(QString path) {
	m_path = path;
}

Project::~Project() {
	if (m_fs) {
		delete m_fs;
	}
}

void Project::create() {
	QDir().mkpath(m_path);

	auto buffSize = 1024;
	auto buff = new uint8_t[buffSize];
	FileSystem32::format(buff, buffSize, true);
	m_fs = createFileSystem(buff, buffSize, true);

	QFile file(m_path + ROM_FILE);
	file.open(QIODevice::WriteOnly);
	file.write((const char*) buff);
	file.close();
}

int Project::open() {
	QFile file(m_path + ROM_FILE);
	auto buffSize = file.size();
	auto buff = new uint8_t[buffSize];
	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		if (file.read((char*) buff, buffSize) > 0) {
			m_fs = createFileSystem((uint8_t*) buff, buffSize, true);
			return 0;
		} else {
			return 1;
		}
	} else {
		return 2;
	}
}

void Project::save() {
	QFile file(m_path + ROM_FILE);
	file.open(QIODevice::WriteOnly);
	file.write((const char*) m_fs->buff(), m_fs->size());
	file.close();
}

FileSystem *Project::romFS() {
	return m_fs;
}

}
}
