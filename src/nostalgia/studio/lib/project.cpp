/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
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

Project::Project(QString path): m_fs(path.toUtf8()) {
	qDebug() << "Project:" << path;
	m_path = path;
}

Project::~Project() {
}

void Project::create() {
	QDir().mkpath(m_path);
}

PassThroughFS *Project::romFs() {
	return &m_fs;
}

void Project::mkdir(QString path) const {
	oxThrowError(m_fs.mkdir(path.toUtf8().data(), true));
	emit updated(path);
}

ox::FileStat Project::stat(QString path) const {
	auto [s, e] = m_fs.stat(path.toUtf8().data());
	oxThrowError(e);
	return s;
}

bool Project::exists(QString path) const {
	return m_fs.stat(path.toUtf8().data()).error == 0;
}

void Project::writeBuff(QString path, uint8_t *buff, size_t buffLen) const {
	oxThrowError(m_fs.write(path.toUtf8().data(), buff, buffLen));
	emit updated(path);
}

std::vector<uint8_t> Project::loadBuff(QString path) const {
	std::vector<uint8_t> buff(stat(path).size);
	const auto csPath = path.toUtf8();
	oxThrowError(m_fs.read(csPath.data(), buff.data(), buff.size()));
	return buff;
}

}
