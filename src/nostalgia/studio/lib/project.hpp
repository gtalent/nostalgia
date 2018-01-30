/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QSharedPointer>

#include <ox/fs/filesystem.hpp>
#include <ox/mc/mc.hpp>

namespace nostalgia {
namespace studio {

class Project: public QObject {
	Q_OBJECT

	private:
		static QString ROM_FILE;

		QString m_path = "";
		ox::FileSystem *m_fs = nullptr;

	public:
		Project(QString path);

		~Project();

		void create();

		int openRomFs();

		int saveRomFs() const;

		ox::FileSystem *romFs();

		int mkdir(QString path) const;

		int write(QString path, uint8_t *buff, size_t buffLen) const;

		/**
		 * Writes a MetalClaw object to the project at the given path.
		 */
		template<typename T>
		int writeObj(QString path, T *obj) const;

		ox::FileStat stat(QString path) const;

	signals:
		void updated(QString path) const;

};

template<typename T>
int Project::writeObj(QString path, T *obj) const {
	int err = 0;
	auto buffLen = 1024 * 1024 * 10;
	QByteArray buff(buffLen, 0);

	// write MetalClaw
	size_t mcSize = 0;
	err |= ox::write((uint8_t*) buff.data(), buffLen, obj, &mcSize);
	if (err) {
		return err;
	}

	// write to FS
	err |= write(path, (uint8_t*) buff.data(), mcSize);
	if (err) {
		return err;
	}

	emit updated(path);

	return err;
}

}
}
