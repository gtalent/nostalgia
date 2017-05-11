/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QList>
#include <QVariant>
#include <QVariant>

namespace nostalgia {
namespace studio {

class OxFSFile {
	private:
		 QList<OxFSFile*> m_childItems;
		 QList<QVariant> m_itemData;
		 OxFSFile *m_parentItem;

	public:
		 explicit OxFSFile(const QList<QVariant> &data, OxFSFile *parentItem = 0);
		 ~OxFSFile();

		 void appendChild(OxFSFile *child);

		 OxFSFile *child(int row);
		 int childCount() const;
		 int columnCount() const;
		 QVariant data(int column) const;
		 int row() const;
		 OxFSFile *parentItem();
};

}
}
