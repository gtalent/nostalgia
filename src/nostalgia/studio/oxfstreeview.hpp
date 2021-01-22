/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QModelIndex>
#include <QVector>
#include <QVariant>

#include <ox/fs/fs.hpp>

namespace nostalgia::studio {

class OxFSFile {
	private:
		OxFSFile *m_parentItem = nullptr;
		QString m_path;
		QVector<OxFSFile*> m_childItems;

	public:
		OxFSFile(ox::PassThroughFS *fs, QString path, OxFSFile *parentItem = nullptr);

		~OxFSFile();

		void appendChild(class OxFSModel *model, QStringList pathItems, QString fullPath);

		OxFSFile *child(int row);

		int childCount() const;

		int columnCount() const;

		QVariant data(int column) const;

		int row() const;

		OxFSFile *parentItem();

		QString name() const;

		QString path() const;
};

class OxFSModel: public QAbstractItemModel {
	Q_OBJECT

	friend OxFSFile;

	private:
		OxFSFile *m_rootItem = nullptr;

	public:
		explicit OxFSModel(ox::PassThroughFS *fs, QObject *parent = 0);

		~OxFSModel();

		QVariant data(const QModelIndex &index, int role) const override;

		Qt::ItemFlags flags(const QModelIndex &index) const override;

		QVariant headerData(int section, Qt::Orientation orientation,
		                    int role = Qt::DisplayRole) const override;

		QModelIndex index(int row, int column,
		                  const QModelIndex &parent = QModelIndex()) const override;

		QModelIndex parent(const QModelIndex &index) const override;

		int rowCount(const QModelIndex &parent = QModelIndex()) const override;

		int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	public slots:
		void updateFile(QString path);

	private:
		void setupModelData(const QStringList &lines, OxFSFile *parent);
};

}
