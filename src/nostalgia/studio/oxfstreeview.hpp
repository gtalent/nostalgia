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
		OxFSFile(ox::FileSystem *fs, const QString &path, OxFSFile *parentItem = nullptr);

		~OxFSFile();

		void appendChild(class OxFSModel *model, QStringList pathItems, QString fullPath);

		OxFSFile *child(int row);

		[[nodiscard]]
		int childCount() const;

		[[nodiscard]]
		int columnCount() const;

		[[nodiscard]]
		QVariant data(int column) const;

		[[nodiscard]]
		int row() const;

		OxFSFile *parentItem();

		[[nodiscard]]
		QString name() const;

		[[nodiscard]]
		QString path() const;
};

class OxFSModel: public QAbstractItemModel {
	Q_OBJECT

	friend OxFSFile;

	private:
		OxFSFile *m_rootItem = nullptr;

	public:
		explicit OxFSModel(ox::FileSystem *fs, QObject *parent = nullptr);

		~OxFSModel() override;

		[[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

		[[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;

		[[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

		[[nodiscard]] QModelIndex index(int row, int column,
		                  const QModelIndex &parent) const override;

		[[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;

		[[nodiscard]] int rowCount(const QModelIndex &parent) const override;

		[[nodiscard]] int columnCount(const QModelIndex &parent) const override;

	public slots:
		void updateFile(const QString &path);

};

}
