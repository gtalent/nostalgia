/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
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
#include <QTabBar>
#include <QTreeView>
#include <QVector>

#include <ox/std/types.hpp>

#include "lib/oxfstreeview.hpp"
#include "lib/plugin.hpp"
#include "lib/project.hpp"

namespace nostalgia {
namespace studio {

struct NostalgiaStudioState {
	QString projectPath;
};

template<typename T>
ox::Error ioOp(T *io, NostalgiaStudioState *obj) {
	ox::Error err = 0;
	io->setFields(1);
	err |= io->op("project_path", &obj->projectPath);
	return err;
}


struct NostalgiaStudioPluginDef {
	QString dir;
	QString libName;
};

template<typename T>
ox::Error ioOp(T *io, NostalgiaStudioPluginDef *obj) {
	ox::Error err = 0;
	io->setFields(2);
	err |= io->op("dir", &obj->dir);
	err |= io->op("lib_name", &obj->libName);
	return err;
}


struct NostalgiaStudioProfile {
	QString appName;
	QString orgName;
	QVector<NostalgiaStudioPluginDef> plugins;
};

template<typename T>
ox::Error ioOp(T *io, NostalgiaStudioProfile *obj) {
	ox::Error err = 0;
	io->setFields(3);
	err |= io->op("app_name", &obj->appName);
	err |= io->op("org_name", &obj->orgName);
	err |= io->op("plugins", &obj->plugins);
	return err;
}


class MainWindow: public QMainWindow {
	Q_OBJECT
	public:
		static const QString EditorSettings;
		static const QString AppTitle;
		static const QString StateFilePath;

	private:
		QString m_profilePath;
		NostalgiaStudioProfile m_profile;
		NostalgiaStudioState m_state;
		QAction *m_importAction = nullptr;
		Context m_ctx;
		QPointer<QMenu> m_viewMenu;
		QVector<std::function<void()>> m_cleanupTasks;
		QVector<QPointer<QDockWidget>> m_dockWidgets;
		QTreeView *m_projectExplorer = nullptr;
		QVector<Plugin*> m_plugins;
		QPointer<OxFSModel> m_oxfsView = nullptr;
		QTabBar *m_tabbar = nullptr;

	public:
		MainWindow(QString profilePath);

		virtual ~MainWindow();

	private:
		void loadPlugins();

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

	public slots:
		void onExit();

	private slots:
		int openProject();

		void showNewWizard();

		void showImportWizard();

		void refreshProjectExplorer(QString path);
};

}
}
