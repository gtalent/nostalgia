/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QDockWidget>
#include <QMainWindow>
#include <QModelIndex>
#include <QPoint>
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <functional>

#include <ox/std/types.hpp>

#include "lib/project.hpp"

namespace nostalgia {
namespace studio {

struct NostalgiaStudioState {
	QString currentProjectPath;
};

template<typename T>
int ioOp(T *io, NostalgiaStudioState *obj) {
	ox::Error err = 0;
	err |= io->op("current_project_path", &obj->currentProjectPath);
	return err;
}


struct NostalgiaStudioProfile {
	QString appName;
};

template<typename T>
int ioOp(T *io, NostalgiaStudioProfile *obj) {
	ox::Error err = 0;
	err |= io->op("app_name", &obj->appName);
	return err;
}


class MainWindow: public QMainWindow {
	Q_OBJECT
	public:
		static const QString EditorSettings;
		static const QString AppTitle;

	private:
		QSharedPointer<Project> m_project;
		QPointer<QMenu> m_viewMenu;
		QVector<std::function<void()>> m_cleanupTasks;
		QVector<QPointer<QDockWidget>> m_dockWidgets;

	public:
		MainWindow(NostalgiaStudioProfile config, QWidget *parent = 0);

		virtual ~MainWindow();

		void openProject(QString);

	private:
		void setupDockWidgets();

		void setupMenu();

		void setupProjectExplorer();

		void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);

		void addAction(QMenu *menu, QString text, QString toolTip,
		               QKeySequence::StandardKey key, const QObject *tgt, const char *cb);

		void addAction(QMenu *menu, QString text, QString toolTip,
		               QKeySequence::StandardKey key, void (*cb)());

		int readSettings(QString path);

		int writeSettings(QString path);

	private slots:
		void openProject();

		void showNewWizard();
};

}
}
