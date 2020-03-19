/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QDialog>
#include <QFileDialog>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPluginLoader>
#include <QScreen>
#include <QSettings>
#include <QTextStream>
#include <QVector>

#include "lib/editor.hpp"
#include "lib/project.hpp"
#include "lib/wizard.hpp"

#include "json.hpp"
#include "mainwindow.hpp"

namespace nostalgia::studio {

const QString MainWindow::StateFilePath = "studio_state.json";

MainWindow::MainWindow(QString profilePath) {
	m_profilePath = profilePath;
	// load in profile file
	QFile file(profilePath);
	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		QTextStream in(&file);
		oxThrowError(readJson(in.readAll(), &m_profile));
		qDebug() << m_profile.appName;
	}

	auto screenSize = QApplication::screens().first()->geometry();

	// set window to 75% of screen width, and center NostalgiaStudioProfile
	constexpr auto sizePct = 0.75;
	resize(screenSize.width() * sizePct, screenSize.height() * sizePct);
	move(-x(), -y());
	move(screenSize.width() * (1 - sizePct) / 2, screenSize.height() * (1 - sizePct) / 2);

	setWindowTitle(m_profile.appName);
	m_ctx.appName = m_profile.appName;
	m_ctx.orgName = m_profile.orgName;

	m_tabs = new QTabWidget(this);
	auto tabBar = m_tabs->tabBar();
	setCentralWidget(m_tabs);
	m_tabs->setTabsClosable(true);
	connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	connect(tabBar, &QTabBar::tabMoved, this, &MainWindow::moveTab);
	connect(tabBar, &QTabBar::currentChanged, this, &MainWindow::changeTab);
	tabBar->setMovable(true);

	setupMenu();
	setupProjectExplorer();
	statusBar(); // setup status bar

	loadPlugins();

	readState();
}

MainWindow::~MainWindow() {
	closeProject();
}

void MainWindow::loadPlugins() {
	for (auto dir : m_profile.pluginsPath) {
		QFileInfo dirInfo(m_profilePath);
		dir = dirInfo.absolutePath() + "/" + dir;
		if (dirInfo.exists()) {
			qDebug() << "Checking plugin directory:" << dir;
			loadPluginDir(dir);
		}
	}
}

void MainWindow::loadPluginDir(QString dir) {
	for (auto pluginPath : QDir(dir).entryList()) {
		pluginPath = dir + '/' + pluginPath;
		if (QLibrary::isLibrary(pluginPath)) {
			loadPlugin(pluginPath);
		}
	}
}

void MainWindow::loadPlugin(QString pluginPath) {
	qDebug() << "Loading plugin:" << pluginPath;
	QPluginLoader loader(pluginPath);
	auto plugin = qobject_cast<Plugin*>(loader.instance());
	if (plugin) {
		m_plugins.push_back(plugin);
		auto editorMakers = plugin->editors(&m_ctx);
		for (const auto &em : editorMakers) {
			for (const auto &ext : em.fileTypes) {
				m_editorMakers[ext] = em;
			}
		}
	} else {
		qInfo() << loader.errorString();
	}
}

void MainWindow::setupMenu() {
	auto menu = menuBar();
	auto fileMenu = menu->addMenu(tr("&File"));
	auto editMenu = menu->addMenu(tr("&Edit"));
	m_viewMenu = menu->addMenu(tr("&View"));

	// New...
	addAction(
		fileMenu,
		tr("&New..."),
		tr(""),
		QKeySequence::New,
		this,
		SLOT(showNewWizard())
	);

	// Import...
	m_importAction = addAction(
		fileMenu,
		tr("&Import..."),
		tr(""),
		this,
		SLOT(showImportWizard())
	);
	m_importAction->setEnabled(false);

	// Open Project
	addAction(
		fileMenu,
		tr("&Open Project"),
		tr(""),
		QKeySequence::Open,
		this,
		SLOT(openProject())
	);

	// Save Project
	m_saveAction = addAction(
		fileMenu,
		tr("&Save File"),
		tr(""),
		QKeySequence::Save,
		this,
		SLOT(saveFile())
	);
	m_saveAction->setEnabled(false);

	// Exit
	addAction(
		fileMenu,
		tr("E&xit"),
		tr("Exit the application"),
		QKeySequence::Quit,
		QApplication::quit
	);

	// Undo
	auto undoAction = m_undoGroup.createUndoAction(this, tr("&Undo"));
	editMenu->addAction(undoAction);
	undoAction->setShortcuts(QKeySequence::Undo);

	// Redo
	auto redoAction = m_undoGroup.createRedoAction(this, tr("&Redo"));
	editMenu->addAction(redoAction);
	redoAction->setShortcuts(QKeySequence::Redo);
}

void MainWindow::setupProjectExplorer() {
	// setup dock
	auto dock = new QDockWidget(tr("Project"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setObjectName("Project Explorer");
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	resizeDocks({dock}, {static_cast<int>(width() * 0.25)}, Qt::Horizontal);

	// setup tree view
	m_projectExplorer = new QTreeView(dock);
	m_projectExplorer->header()->hide();
	dock->setWidget(m_projectExplorer);

	connect(m_projectExplorer, &QTreeView::activated, this, &MainWindow::openFileSlot);
}

void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockWidget) {
	QMainWindow::addDockWidget(area, dockWidget);
	m_viewMenu->addAction(dockWidget->toggleViewAction());
	m_dockWidgets.push_back(dockWidget);
}

QAction *MainWindow::addAction(QMenu *menu, QString text, QString toolTip, const QObject *tgt, const char *cb) {
	auto action = menu->addAction(text);
	action->setStatusTip(toolTip);
	auto conn = connect(action, SIGNAL(triggered()), tgt, cb);
	return action;
}

QAction *MainWindow::addAction(QMenu *menu, QString text, QString toolTip,
                               QKeySequence::StandardKey key, const QObject *tgt, const char *cb) {
	auto action = menu->addAction(text);
	action->setShortcuts(key);
	action->setStatusTip(toolTip);
	auto conn = connect(action, SIGNAL(triggered()), tgt, cb);
	return action;
}

QAction *MainWindow::addAction(QMenu *menu, QString text, QString toolTip,
                               QKeySequence::StandardKey key, void (*cb)()) {
	auto action = menu->addAction(text);
	action->setShortcuts(key);
	action->setStatusTip(toolTip);
	auto conn = connect(action, &QAction::triggered, cb);
	return action;
}

int MainWindow::readState() {
	int err = 0;

	QSettings settings(m_profile.orgName, m_profile.appName);
	settings.beginGroup("MainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
	auto json = settings.value("json").toString();
	err |= readJson(json, &m_state);
	settings.endGroup();

	openProject(m_state.projectPath);

	return err;
}

void MainWindow::writeState() {
	// generate JSON for application specific state info
	QString json;
	writeJson(&json, &m_state);

	QSettings settings(m_profile.orgName, m_profile.appName);
	settings.beginGroup("MainWindow");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	settings.setValue("json", json);
	settings.endGroup();
}

/**
 * Read open editor tabs for current project.
 */
QStringList MainWindow::readTabs() {
	QStringList tabs;
	QSettings settings(m_profile.orgName, m_profile.appName);
	settings.beginReadArray("MainWindow/Project:" + m_state.projectPath);
	auto size = settings.beginReadArray("openEditors");
	for (int i = 0; i < size; i++) {
		settings.setArrayIndex(i);
		tabs.append(settings.value("filePath").toString());
	}
	settings.endArray();
	return tabs;
}

/**
 * Write open editor tabs for current project.
 */
void MainWindow::writeTabs(QStringList tabs) {
	QSettings settings(m_profile.orgName, m_profile.appName);
	settings.beginWriteArray("MainWindow/Project:" + m_state.projectPath + "/openEditors");
	for (int i = 0; i < tabs.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("filePath", tabs[i]);
	}
	settings.endArray();
}

void MainWindow::openProject(QString projectPath) {
	closeProject();
	auto project = new Project(projectPath);
	if (m_ctx.project) {
		delete m_ctx.project;
		m_ctx.project = nullptr;
	}
	m_ctx.project = project;
	m_oxfsView = new OxFSModel(project->romFs());
	m_projectExplorer->setModel(m_oxfsView);
	connect(m_ctx.project, &Project::fileUpdated, m_oxfsView, &OxFSModel::updateFile);
	m_importAction->setEnabled(true);
	m_state.projectPath = projectPath;
	// reopen tabs
	auto openTabs = readTabs();
	for (auto t : openTabs) {
		try {
			openFile(t, true);
		} catch (ox::Error err) {
			qInfo() << "Error opening tab:" << static_cast<int>(err);
			oxTrace("nostalgia::studio::MainWindow::openProject") << "Error opening tab:" << static_cast<int>(err);
		} catch (...) {
			qInfo() << "Error opening tab";
			oxTrace("nostalgia::studio::MainWindow::openProject") << "Error opening tab";
		}
	}
	qInfo() << "Open project:" << projectPath;
}

void MainWindow::closeProject() {
	// delete tabs
	while (m_tabs->count()) {
		auto tab = static_cast<studio::Editor*>(m_tabs->widget(0));
		m_undoGroup.removeStack(tab->undoStack());
		m_tabs->removeTab(0);
		delete tab;
	}

	if (m_ctx.project) {
		disconnect(m_ctx.project, &Project::fileUpdated, m_oxfsView, &OxFSModel::updateFile);

		delete m_ctx.project;
		m_ctx.project = nullptr;

		delete m_oxfsView;
		m_oxfsView = nullptr;
	}
	if (m_projectExplorer->model()) {
		delete m_projectExplorer->model();
	}
	m_projectExplorer->setModel(nullptr);

	m_importAction->setEnabled(false);

	m_state.projectPath = "";
}

void MainWindow::openFile(QString path, bool force) {
	if (!force && readTabs().contains(path)) {
		return;
	}
	const auto dotIdx = path.lastIndexOf('.') + 1;
	if (dotIdx == -1) {
		return;
	}
	const auto ext = path.mid(dotIdx);
	const auto lastSlash = path.lastIndexOf('/') + 1;
	auto tabName = path.mid(lastSlash);
	if (m_editorMakers.contains(ext)) {
		auto tab = m_editorMakers[ext].make(path);
		m_tabs->addTab(tab, tabName);
		m_undoGroup.addStack(tab->undoStack());
		// save new tab to application state
		auto openTabs = readTabs();
		if (!openTabs.contains(path)) {
			openTabs.append(path);
		}
		writeTabs(openTabs);
	}
}

void MainWindow::onExit() {
	writeState();
}


// private slots

void MainWindow::openProject() {
	auto projectPath = QFileDialog::getExistingDirectory(this, tr("Select Project Directory..."), QDir::homePath());
	if (projectPath != "") {
		openProject(projectPath);
		writeState();
	}
}

void MainWindow::openFileSlot(QModelIndex file) {
	auto path = static_cast<OxFSFile*>(file.internalPointer())->path();
	return openFile(path);
}

void MainWindow::showNewWizard() {
	const QString ProjectName = "projectName";
	const QString ProjectPath = "projectPath";
	Wizard wizard(tr("New..."));
	auto ws = new WizardSelect();
	wizard.addPage(ws);

	// add new project wizard
	ws->addOption(
		WizardMaker {
			tr("Project"),

			[&wizard, ProjectName, ProjectPath]() {
				QVector<QWizardPage*> pgs;
				auto pg = new WizardFormPage();
				pg->addLineEdit(tr("Project &Name:"), ProjectName + "*", "", [ProjectPath, pg, &wizard](QString projectName) {
						auto projectPath = wizard.field(ProjectPath).toString();
						auto path = projectPath + "/" + projectName;
						if (!QDir(path).exists()) {
							return 0;
						} else {
							pg->showValidationError(tr("This project directory already exists."));
							return 1;
						}
					}
				);
				pg->addPathBrowse(tr("Project &Path:"), ProjectPath + "*", QDir::homePath(), QFileDialog::Directory);
				pgs.push_back(pg);
				pgs.push_back(new WizardConclusionPage(tr("Creating project: %1/%2"), {ProjectPath, ProjectName}));
				return pgs;
			},

			[this, ProjectName, ProjectPath](QWizard *wizard) {
				auto projectName = wizard->field(ProjectName).toString();
				auto projectPath = wizard->field(ProjectPath).toString();
				qInfo() << "Project creation: final step";
				if (QDir(projectPath).exists()) {
					auto path = projectPath + "/" + projectName;
					if (!QDir(path).exists()) {
						Project(path).create();
						openProject(path);
						qInfo() << "Project creation successful:" << path;
						return 0;
					} else {
						qInfo() << "Project file exists:" << path;
						return 1;
					}
				} else {
					qInfo() << "Project destination directory does not exist:" << projectPath;
					return 2;
				}
			}
		}
	);

	// add plugin options
	for (auto p : m_plugins) {
		for (auto w : p->newWizards(&m_ctx)) {
			ws->addOption(w);
		}
	}

	wizard.show();
	wizard.exec();
}

void MainWindow::saveFile() {
	m_currentEditor->save();
}

void MainWindow::closeTab(int idx) {
	auto tab = static_cast<studio::Editor*>(m_tabs->widget(idx));
	m_undoGroup.removeStack(tab->undoStack());
	m_tabs->removeTab(idx);
	delete tab;

	// remove from open tabs list
	auto tabs = readTabs();
	tabs.removeAt(idx);
	writeTabs(tabs);
}

void MainWindow::moveTab(int from, int to) {
	// move tab in open tabs list
	auto tabs = readTabs();
	tabs.move(from, to);
	writeTabs(tabs);
}

void MainWindow::changeTab(int idx) {
	auto tab = dynamic_cast<studio::Editor*>(m_tabs->widget(idx));
	if (m_currentEditor) {
		disconnect(m_currentEditor, &Editor::unsavedChangesUpdate, m_saveAction, &QAction::setEnabled);
		disconnect(m_currentEditor, &Editor::unsavedChangesUpdate, this, &MainWindow::markUnsavedChanges);
	}
	m_currentEditor = tab;
	if (m_currentEditor) {
		connect(m_currentEditor, &Editor::unsavedChangesUpdate, m_saveAction, &QAction::setEnabled);
		connect(m_currentEditor, &Editor::unsavedChangesUpdate, this, &MainWindow::markUnsavedChanges);
		m_undoGroup.setActiveStack(tab->undoStack());
	} else {
		m_undoGroup.setActiveStack(nullptr);
	}
}

void MainWindow::markUnsavedChanges(bool unsavedChanges) {
	auto idx = m_tabs->indexOf(m_currentEditor);
	if (idx > -1) {
		const auto path = m_currentEditor->itemName();
		const auto lastSlash = path.lastIndexOf('/') + 1;
		const auto tabName = path.mid(lastSlash);
		if (unsavedChanges) {
			m_tabs->setTabText(idx, tabName + "*");
		} else {
			m_tabs->setTabText(idx, tabName);
		}
	}
}

void MainWindow::showImportWizard() {
	const QString TILESHEET_NAME = "projectName";
	const QString IMPORT_PATH = "projectPath";
	const QString BPP = "bpp";
	Wizard wizard(tr("Import..."));
	auto ws = new WizardSelect();
	wizard.addPage(ws);

	for (auto p : m_plugins) {
		for (auto w : p->importWizards(&m_ctx)) {
			ws->addOption(w);
		}
	}

	wizard.show();
	wizard.exec();
}

void MainWindow::refreshProjectExplorer(QString path) {
	if (m_oxfsView) {
		m_oxfsView->updateFile(path);
	}
}

}
