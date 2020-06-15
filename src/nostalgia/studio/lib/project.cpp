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

QString filePathToName(QString path, QString prefix, QString suffix) {
	const auto begin = prefix.size();
	const auto end = path.size() - (suffix.size() + prefix.size());
	return path.mid(begin, end);
}


Project::Project(QString path): m_fs(path.toUtf8()) {
	qDebug() << "Project:" << path;
	m_path = path;
}

Project::~Project() {
}

void Project::create() {
	QDir().mkpath(m_path);
}

ox::PassThroughFS *Project::romFs() {
	return &m_fs;
}

void Project::mkdir(QString path) const {
	oxThrowError(m_fs.mkdir(path.toUtf8().data(), true));
	emit fileUpdated(path);
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
	emit fileUpdated(path);
}

std::vector<char> Project::loadBuff(QString path) const {
	std::vector<char> buff(stat(path).size);
	const auto csPath = path.toUtf8();
	oxThrowError(m_fs.read(csPath.data(), buff.data(), buff.size()));
	return buff;
}

void Project::procDir(QStringList &paths, QString path) const {
	oxThrowError(m_fs.ls(path.toUtf8(), [&](QString name, ox::InodeId_t) {
		auto fullPath = path + "/" + name;
		auto [stat, err] = m_fs.stat(fullPath.toUtf8().data());
		oxReturnError(err);
		switch (stat.fileType) {
			case ox::FileType_NormalFile:
				paths.push_back(fullPath);
				break;
			case ox::FileType_Directory:
				procDir(paths, fullPath);
				break;
		}
		return OxError(0);
	}));
}

QStringList Project::listFiles(QString path) const {
	QStringList paths;
	procDir(paths, path);
	return paths;
}

}
