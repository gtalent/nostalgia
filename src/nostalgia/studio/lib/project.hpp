/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
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

namespace nostalgia::studio {

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

		ox::Error openRomFs();

		ox::Error saveRomFs() const;

		ox::PassThroughFS *romFs();

		ox::Error mkdir(QString path) const;

		ox::Error write(QString path, uint8_t *buff, size_t buffLen) const;

		/**
		 * Writes a MetalClaw object to the project at the given path.
		 */
		template<typename T>
		ox::Error writeObj(QString path, T *obj) const;

		ox::ValErr<ox::FileStat> stat(QString path) const;

	signals:
		void updated(QString path) const;

};

template<typename T>
ox::Error Project::writeObj(QString path, T *obj) const {
	auto buffLen = 1024 * 1024 * 10;
	QByteArray buff(buffLen, 0);

	// write MetalClaw
	size_t mcSize = 0;
	oxReturnError(ox::writeMC(reinterpret_cast<uint8_t*>(buff.data()), buffLen, obj, &mcSize));
	// write to FS
	oxReturnError(write(path, reinterpret_cast<uint8_t*>(buff.data()), mcSize));

	emit updated(path);

	return OxError(0);
}

}
