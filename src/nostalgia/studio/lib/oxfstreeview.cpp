/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QVector>

#include "oxfstreeview.hpp"

namespace nostalgia {
namespace studio {

using namespace ox;

OxFSFile::OxFSFile(FileSystem *fs, QString path, OxFSFile *parentItem) {
	m_fs = fs;
	m_path = path;
	m_parentItem = parentItem;
}

OxFSFile::~OxFSFile() {
	qDeleteAll(m_childItems);
}

void OxFSFile::appendChild(OxFSFile*) {
}

OxFSFile *OxFSFile::child(int row) {
	if (m_fs) {
		QVector<DirectoryListing<QString>> ls;
		m_fs->ls(m_path.toUtf8(), &ls);
		auto ch = new OxFSFile(m_fs, m_path + "/" + ls[row].name, this);
		m_childItems.push_back(ch);
		return ch;
	} else {
		return nullptr;
	}
}

int OxFSFile::childCount() const {
	if (m_fs) {
		QVector<DirectoryListing<QString>> ls;
		m_fs->ls(m_path.toUtf8(), &ls);
		return ls.size();
	} else {
		return 0;
	}
}

int OxFSFile::columnCount() const {
	return 1;
}

QVariant OxFSFile::data(int) const {
	return m_path.mid(m_path.lastIndexOf('/') + 1);
}

int OxFSFile::row() const {
	if (m_parentItem) {
		return m_parentItem->m_childItems.indexOf(const_cast<OxFSFile*>(this));
	} else {
		return 0;
	}
}

OxFSFile *OxFSFile::parentItem() {
	return m_parentItem;
}


// OxFSModel

OxFSModel::OxFSModel(FileSystem *fs, QObject *parent) {
	m_rootItem = new OxFSFile(fs, "");
}

OxFSModel::~OxFSModel() {
	if (m_rootItem) {
		delete m_rootItem;
	}
}

QVariant OxFSModel::data(const QModelIndex &index, int role) const {
	if (!index.isValid() || role != Qt::DisplayRole) {
		return QVariant();
	} else {
		OxFSFile *item = static_cast<OxFSFile*>(index.internalPointer());
		return item->data(index.column());
	}
}

Qt::ItemFlags OxFSModel::flags(const QModelIndex &index) const {
	if (!index.isValid()) {
		return 0;
	} else {
		return QAbstractItemModel::flags(index);
	}
}

QVariant OxFSModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		return m_rootItem->data(section);
	} else {
		return QVariant();
	}
}

QModelIndex OxFSModel::index(int row, int column, const QModelIndex &parent) const {
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	OxFSFile *parentItem;
	if (!parent.isValid()) {
		parentItem = m_rootItem;
	} else {
		parentItem = static_cast<OxFSFile*>(parent.internalPointer());
	}

	OxFSFile *childItem = parentItem->child(row);
	if (childItem) {
		return createIndex(row, column, childItem);
	} else {
		return QModelIndex();
	}
}

QModelIndex OxFSModel::parent(const QModelIndex &index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}

	OxFSFile *childItem = static_cast<OxFSFile*>(index.internalPointer());
	OxFSFile *parentItem = childItem->parentItem();
	if (parentItem == m_rootItem) {
		return QModelIndex();
	}

	return createIndex(parentItem->row(), 0, parentItem);
}

int OxFSModel::rowCount(const QModelIndex &parent) const {
	if (parent.column() > 0) {
		return 0;
	}

	OxFSFile *parentItem;
	if (!parent.isValid()) {
		parentItem = m_rootItem;
	} else {
		parentItem = static_cast<OxFSFile*>(parent.internalPointer());
	}

	return parentItem->childCount();
}

int OxFSModel::columnCount(const QModelIndex &parent) const {
	if (parent.isValid()) {
		return static_cast<OxFSFile*>(parent.internalPointer())->columnCount();
	} else {
		return m_rootItem->columnCount();
	}
}

void OxFSModel::setupModelData(const QStringList &lines, OxFSFile *parent) {
}

}
}
