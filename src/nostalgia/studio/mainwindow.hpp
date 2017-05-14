/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <functional>

#include <QDockWidget>
#include <QMainWindow>
#include <QModelIndex>
#include <QPoint>
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QTreeView>
#include <QVector>

#include <ox/std/types.hpp>

#include "lib/project.hpp"

namespace nostalgia {
namespace studio {

struct NostalgiaStudioState {
	QString projectPath;
};

template<typename T>
int ioOp(T *io, NostalgiaStudioState *obj) {
	ox::Error err = 0;
	io->setFields(1);
	err |= io->op("project_path", &obj->projectPath);
	return err;
}


struct NostalgiaStudioProfile {
	QString appName;
};

template<typename T>
int ioOp(T *io, NostalgiaStudioProfile *obj) {
	ox::Error err = 0;
	io->setFields(1);
	err |= io->op("app_name", &obj->appName);
	return err;
}


class MainWindow: public QMainWindow {
	Q_OBJECT
	public:
		static const QString EditorSettings;
		static const QString AppTitle;
		static const QString StateFilePath;

	private:
		NostalgiaStudioState m_state;
		QAction *m_importAction = nullptr;
		QSharedPointer<Project> m_project;
		QPointer<QMenu> m_viewMenu;
		QVector<std::function<void()>> m_cleanupTasks;
		QVector<QPointer<QDockWidget>> m_dockWidgets;
		QTreeView *m_projectExplorer = nullptr;

	public:
		MainWindow(NostalgiaStudioProfile config, QWidget *parent = 0);

		virtual ~MainWindow();

	private:
		void setupDockWidgets();

		void setupMenu();

		void setupProjectExplorer();

		void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);

		QAction *addAction(QMenu *menu, QString text, QString toolTip, const QObject *tgt, const char *cb);

		QAction *addAction(QMenu *menu, QString text, QString toolTip,
		                   QKeySequence::StandardKey key, const QObject *tgt, const char *cb);

		QAction *addAction(QMenu *menu, QString text, QString toolTip,
		                   QKeySequence::StandardKey key, void (*cb)());

		int readState(QString path = StateFilePath);

		int writeState(QString path = StateFilePath);

		int openProject(QString);

		int closeProject();

	private slots:
		int openProject();

		void showNewWizard();

		void showImportWizard();
};

}
}
