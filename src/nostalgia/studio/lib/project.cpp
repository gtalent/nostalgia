/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
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


Project::Project(QString path): m_fs(std::make_unique<ox::PassThroughFS>(path.toUtf8())) {
	qDebug() << "Project:" << path;
	m_path = path;
}

Project::~Project() = default;

void Project::create() {
	QDir().mkpath(m_path);
}

ox::FileSystem *Project::romFs() {
	return m_fs.get();
}

void Project::mkdir(QString path) const {
	oxThrowError(m_fs->mkdir(path.toUtf8().data(), true));
	emit fileUpdated(path);
}

ox::FileStat Project::stat(QString path) const {
	oxRequireT(s, m_fs->stat(path.toUtf8().data()));
	return s;
}

bool Project::exists(QString path) const {
	return m_fs->stat(path.toUtf8().data()).error == 0;
}

void Project::writeBuff(QString path, uint8_t *buff, size_t buffLen) const {
	oxThrowError(m_fs->write(path.toUtf8().data(), buff, buffLen));
	emit fileUpdated(path);
}

ox::Buffer Project::loadBuff(QString path) const {
	const auto csPath = path.toUtf8();
	oxRequireMT(buff, m_fs->read(csPath.data()));
	return std::move(buff);
}

void Project::lsProcDir(QStringList *paths, QString path) const {
	oxRequireT(files, m_fs->ls(path.toUtf8()));
	for (const auto &name : files) {
		const auto fullPath = path + "/" + name.c_str();
		oxRequireT(stat, m_fs->stat(fullPath.toUtf8().data()));
		switch (stat.fileType) {
			case ox::FileType_NormalFile:
				paths->push_back(fullPath);
				break;
			case ox::FileType_Directory:
				lsProcDir(paths, fullPath);
				break;
		}
	}
}

QStringList Project::listFiles(QString path) const {
	QStringList paths;
	lsProcDir(&paths, path);
	return paths;
}

}
