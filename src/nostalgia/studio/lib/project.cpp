/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QByteArray>
#include <QDir>
#include <project.hpp>

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

	m_romBuff = QSharedPointer<QByteArray>(new QByteArray(1024, 0));
	FileSystem32::format((uint8_t*) m_romBuff->data(), m_romBuff->size(), true);
	m_fs = createFileSystem((uint8_t*) m_romBuff->data(), m_romBuff->size());

	QFile file(m_path + ROM_FILE);
	file.open(QIODevice::WriteOnly);
	file.write(*m_romBuff);
	file.close();
}

int Project::open() {
	QFile file(m_path + ROM_FILE);
	m_romBuff = QSharedPointer<QByteArray>(new QByteArray(file.size(), 0));
	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		if (file.read(m_romBuff->data(), file.size()) > 0) {
			m_fs = createFileSystem((uint8_t*) m_romBuff->data(), m_romBuff->size());
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
	file.write(*m_romBuff);
	file.close();
}

FileSystem *Project::romFS() {
	return m_fs;
}

}
}
