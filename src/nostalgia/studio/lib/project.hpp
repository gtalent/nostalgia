/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <memory>

#include <QSharedPointer>

#include <ox/fs/fs.hpp>
#include <ox/mc/mc.hpp>

namespace nostalgia {
namespace studio {

class Project: public QObject {
	Q_OBJECT

	private:
		static QString ROM_FILE;

		QString m_path = "";
		std::unique_ptr<uint8_t[]> m_fsBuff;
		mutable ox::PassThroughFS m_fs;

	public:
		Project(QString path);

		~Project();

		void create();

		int openRomFs();

		int saveRomFs() const;

		ox::PassThroughFS *romFs();

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
	err |= ox::writeMC((uint8_t*) buff.data(), buffLen, obj, &mcSize);
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
