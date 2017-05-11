/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <ox/fs/filesystem.hpp>

namespace nostalgia {
namespace studio {

class Project: public QObject {
	Q_OBJECT

	private:
		static QString ROM_FILE;

		QString m_path = "";
		QByteArray *m_romBuff = nullptr;
		ox::fs::FileSystem *m_fs = nullptr;

	public:
		Project(QString path);

		~Project();

		void create();

		int open();

		void save();
};

}
}
