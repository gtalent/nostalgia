/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
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
#include "lib/module.hpp"
#include "lib/project.hpp"

#include "builtinmodules.hpp"
#include "oxfstreeview.hpp"

namespace nostalgia::studio {

struct NostalgiaStudioState {
	QString projectPath;
};

template<typename T>
ox::Error model(T *io, NostalgiaStudioState *obj) {
	oxReturnError(io->setTypeInfo("net.drinkingtea.nostalgia.studio.NostalgiaStudioState", 1));
	oxReturnError(io->field("project_path", &obj->projectPath));
	return OxError(0);
}

struct NostalgiaStudioProfile {
	QString appName;
	QString orgName;
	QVector<QString> modulesPath;
};

template<typename T>
ox::Error model(T *io, NostalgiaStudioProfile *obj) {
	oxReturnError(io->setTypeInfo("net.drinkingtea.nostalgia.studio.NostalgiaStudioProfile", 3));
	oxReturnError(io->field("app_name", &obj->appName));
	oxReturnError(io->field("org_name", &obj->orgName));
	oxReturnError(io->field("modules_path", &obj->modulesPath));
	return OxError(0);
}


class MainWindow: public QMainWindow {
	Q_OBJECT

	private:
		QString m_profilePath;
		NostalgiaStudioProfile m_profile;
		NostalgiaStudioState m_state;
		QAction *m_importAction = nullptr;
		QAction *m_saveAction = nullptr;
		QAction *m_exportAction = nullptr;
		QAction *m_cutAction = nullptr;
		QAction *m_copyAction = nullptr;
		QAction *m_pasteAction = nullptr;
		Context m_ctx;
		QPointer<QMenu> m_viewMenu;
		QVector<QPointer<QDockWidget>> m_dockWidgets;
		QTreeView *m_projectExplorer = nullptr;
		QVector<Module*> m_modules;
		QHash<QString, EditorMaker> m_editorMakers;
		QPointer<OxFSModel> m_oxfsView = nullptr;
		QTabWidget *m_tabs = nullptr;
		QUndoGroup m_undoGroup;
		Editor *m_currentEditor = nullptr;

	public:
		explicit MainWindow(QString profilePath);

		~MainWindow() override;

	private:
		void loadModules();

		void loadModuleDir(QString path);

		void loadModule(QString path);

		void loadModule(Module *module);

		void setupMenu();

		void setupProjectExplorer();

		void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);

		static QAction *addAction(QMenu *menu, QString text, QString toolTip, const QObject *tgt, const char *cb);

		static QAction *addAction(QMenu *menu, QString text, QString toolTip,
		                   QKeySequence::StandardKey key, const QObject *tgt, const char *cb);

		static QAction *addAction(QMenu *menu, QString text, QString toolTip,
		                   QKeySequence::StandardKey key, void (*cb)());

		int readState();

		void writeState();

		/**
		 * Read open editor tabs for current project.
		 */
		[[nodiscard]] QStringList readTabs() const;

		/**
		 * Write open editor tabs for current project.
		 */
		void writeTabs(QStringList tabs) const;

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

		void exportFile();

		void cutAction();

		void copyAction();

		void pasteAction();

		void closeTab(int idx);

		void moveTab(int from, int to);

		void changeTab(int idx);

		void markUnsavedChanges(bool);

		void showNewWizard();

		void showImportWizard();

		void refreshProjectExplorer(QString path);

};

}
