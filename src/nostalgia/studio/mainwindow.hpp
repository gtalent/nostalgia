/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
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
#include <QTabWidget>
#include <QTreeView>
#include <QUndoGroup>
#include <QVector>

#include <ox/std/types.hpp>

#include "lib/context.hpp"
#include "lib/plugin.hpp"
#include "lib/project.hpp"

#include "oxfstreeview.hpp"

namespace nostalgia::studio {

struct NostalgiaStudioState {
	QString projectPath;
};

template<typename T>
ox::Error model(T *io, NostalgiaStudioState *obj) {
	auto err = OxError(0);
	oxReturnError(io->setTypeInfo("NostalgiaStudioState", 1));
	oxReturnError(io->field("project_path", &obj->projectPath));
	return err;
}

struct NostalgiaStudioProfile {
	QString appName;
	QString orgName;
	QVector<QString> pluginsPath;
};

template<typename T>
ox::Error model(T *io, NostalgiaStudioProfile *obj) {
	auto err = OxError(0);
	io->setTypeInfo("NostalgiaStudioProfile", 3);
	oxReturnError(io->field("app_name", &obj->appName));
	oxReturnError(io->field("org_name", &obj->orgName));
	oxReturnError(io->field("plugins_path", &obj->pluginsPath));
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
		QAction *m_saveAction = nullptr;
		Context m_ctx;
		QPointer<QMenu> m_viewMenu;
		QVector<QPointer<QDockWidget>> m_dockWidgets;
		QTreeView *m_projectExplorer = nullptr;
		QVector<Plugin*> m_plugins;
		QHash<QString, EditorMaker> m_editorMakers;
		QPointer<OxFSModel> m_oxfsView = nullptr;
		QTabWidget *m_tabs = nullptr;
		QUndoGroup m_undoGroup;
		Editor* m_currentEditor = nullptr;

	public:
		MainWindow(QString profilePath);

		virtual ~MainWindow();

	private:
		void loadPlugins();

		void loadPluginDir(QString path);

		void loadPlugin(QString path);

		void setupMenu();

		void setupProjectExplorer();

		void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);

		QAction *addAction(QMenu *menu, QString text, QString toolTip, const QObject *tgt, const char *cb);

		QAction *addAction(QMenu *menu, QString text, QString toolTip,
		                   QKeySequence::StandardKey key, const QObject *tgt, const char *cb);

		QAction *addAction(QMenu *menu, QString text, QString toolTip,
		                   QKeySequence::StandardKey key, void (*cb)());

		int readState();

		void writeState();

		/**
		 * Read open editor tabs for current project.
		 */
		QStringList readTabs();

		/**
		 * Write open editor tabs for current project.
		 */
		void writeTabs(QStringList tabs);

		void openProject(QString);

		void closeProject();

		/**
		 * @param force forces the reopening of the file even if it is already open
		 */
		void openFile(QString path, bool force = false);

	public slots:
		void onExit();

	private slots:
		void openProject();

		void openFileSlot(QModelIndex);

		void saveFile();

		void closeTab(int idx);

		void moveTab(int from, int to);

		void changeTab(int idx);

		void markUnsavedChanges(bool);

		void showNewWizard();

		void showImportWizard();

		void refreshProjectExplorer(QString path);

};

}
