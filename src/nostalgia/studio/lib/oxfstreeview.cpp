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
	m_path = path;
	m_parentItem = parentItem;

	// find children
	if (fs) {
		QVector<DirectoryListing<QString>> ls;
		if (fs->stat((const char*) m_path.toUtf8()).fileType == FileType_Directory) {
			fs->ls(m_path.toUtf8(), &ls);
			qSort(ls);
		}
		for (auto v : ls) {
			if (v.name != "." && v.name != "..") {
				auto ch = new OxFSFile(fs, m_path + "/" + v.name, this);
				m_childItems.push_back(ch);
			}
		}
	}
}

OxFSFile::~OxFSFile() {
	qDeleteAll(m_childItems);
}

void OxFSFile::appendChild(OxFSModel *model, QStringList pathItems, QString currentPath) {
	if (pathItems.size()) {
		auto target = pathItems[0];
		currentPath += "/" + target;
		OxFSFile *nextItem = nullptr;
		int index = m_childItems.size();
		for (int i = 0; i < m_childItems.size(); i++) {
			if (m_childItems[i]->name() >= target) {
				index = i;
				break;
			}
		}

		if (m_childItems.size() == index || m_childItems[index]->name() != target) {
			auto idx = model->createIndex(row(), 0, this);
			model->beginInsertRows(idx, index, index);
			nextItem = new OxFSFile(nullptr, currentPath, this);
			m_childItems.insert(index, nextItem);
			model->endInsertRows();
		}

		nextItem = m_childItems[index];

		nextItem->appendChild(model, pathItems.mid(1), currentPath);
	}
}

OxFSFile *OxFSFile::child(int row) {
	if (row < m_childItems.size()) {
		return m_childItems[row];
	} else {
		return nullptr;
	}
}

int OxFSFile::childCount() const {
	return m_childItems.size();
}

int OxFSFile::columnCount() const {
	return 1;
}

QVariant OxFSFile::data(int) const {
	return name();
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

QString OxFSFile::name() const {
	return m_path.mid(m_path.lastIndexOf("/") + 1);
}


// OxFSModel

OxFSModel::OxFSModel(FileSystem *fs, QObject *parent) {
	m_rootItem = new OxFSFile(fs, "");
}

OxFSModel::~OxFSModel() {
	if (m_rootItem) {
		delete m_rootItem;
		m_rootItem = nullptr;
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

void OxFSModel::updateFile(QString path) {
	auto pathItems = path.split("/").mid(1);
	m_rootItem->appendChild(this, pathItems, "");
}

void OxFSModel::setupModelData(const QStringList &lines, OxFSFile *parent) {
}

}
}
