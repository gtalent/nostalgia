/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QDir>
#include <QDebug>

#include "project.hpp"

namespace nostalgia::studio {

using namespace ox;

QString Project::ROM_FILE = "/ROM.oxfs";

Project::Project(QString path): m_fs(path.toUtf8()) {
	qDebug() << "Project:" << path;
	m_path = path;
}

Project::~Project() {
}

void Project::create() {
	QDir().mkpath(m_path);
}

ox::Error Project::openRomFs() {
	QFile file(m_path + ROM_FILE);
	auto buffSize = file.size();
	auto buff = std::make_unique<uint8_t[]>(buffSize);
	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		if (file.read(reinterpret_cast<char*>(buff.get()), buffSize) > 0) {
			m_fsBuff = std::move(buff);
			if (m_fs.valid()) {
				return OxError(0);
			} else {
				return OxError(1);
			}
		} else {
			return OxError(2);
		}
	} else {
		return OxError(3);
	}
}

ox::Error Project::saveRomFs() const {
	ox::Error err(0);
	//QFile file(m_path + ROM_FILE);
	//err |= file.open(QIODevice::WriteOnly) == false;
	//err |= file.write((const char*) m_fsBuff.get(), m_fs.size()) == -1;
	//file.close();
	return err;
}

PassThroughFS *Project::romFs() {
	return &m_fs;
}

ox::Error Project::mkdir(QString path) const {
	auto err = m_fs.mkdir(path.toUtf8().data(), true);
	emit updated(path);
	return err;
}

ox::Error Project::write(QString path, uint8_t *buff, size_t buffLen) const {
	auto err = m_fs.write(path.toUtf8().data(), buff, buffLen);
	emit updated(path);
	return err;
}

ox::ValErr<ox::FileStat> Project::stat(QString path) const {
	return m_fs.stat(path.toUtf8().data());
}

}
