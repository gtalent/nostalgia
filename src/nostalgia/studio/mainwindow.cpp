/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
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
#include <QSettings>
#include <QTextStream>
#include <QVector>

#include "lib/json.hpp"
#include "lib/oxfstreeview.hpp"
#include "lib/project.hpp"
#include "lib/wizard.hpp"

#include "mainwindow.hpp"

namespace nostalgia {
namespace studio {

const QString MainWindow::StateFilePath = "studio_state.json";

MainWindow::MainWindow(QString profilePath) {
	m_profilePath = profilePath;
	// load in profile file
	QFile file(profilePath);
	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		QTextStream in(&file);
		readJson(in.readAll(), &m_profile);
	}

	auto screenSize = QApplication::desktop()->screenGeometry();

	// set window to 75% of screen width, and center NostalgiaStudioProfile
	auto sizePct = 0.75;
	resize(screenSize.width() * sizePct, screenSize.height() * sizePct);
	move(-x(), -y());
	move(screenSize.width() * (1 - sizePct) / 2, screenSize.height() * (1 - sizePct) / 2);

	setWindowTitle(m_profile.appName);

	m_tabbar = new QTabBar(this);
	setCentralWidget(m_tabbar);

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
	} else {
		qInfo() << loader.errorString();
	}
}

void MainWindow::setupMenu() {
	auto menu = menuBar();
	auto fileMenu = menu->addMenu(tr("&File"));
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

	// Exit
	addAction(
		fileMenu,
		tr("E&xit"),
		tr("Exit the application"),
		QKeySequence::Quit,
		QApplication::quit
	);
}

void MainWindow::setupProjectExplorer() {
	// setup dock
	auto dock = new QDockWidget(tr("Project"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setObjectName("Project Explorer");
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	resizeDocks({dock}, {(int) (width() * 0.25)}, Qt::Horizontal);

	// setup tree view
	m_projectExplorer = new QTreeView(dock);
	m_projectExplorer->header()->hide();
	dock->setWidget(m_projectExplorer);
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

int MainWindow::readState(QString path) {
	int err = 0;

	QSettings settings(m_profile.orgName, m_profile.appName);
	settings.beginGroup("MainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
	auto json = settings.value("json").toString();
	err |= readJson(json, &m_state);
	settings.endGroup();

	err |= openProject(m_state.projectPath);

	return err;
}

int MainWindow::writeState(QString path) {
	int err = 0;

	// generate JSON for application specific state info
	QString json;
	err |= writeJson(&json, &m_state);

	QSettings settings(m_profile.orgName, m_profile.appName);
	settings.beginGroup("MainWindow");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	settings.setValue("json", json);
	settings.endGroup();

	return err;
}

int MainWindow::openProject(QString projectPath) {
	auto err = closeProject();
	auto project = new Project(projectPath);
	err |= project->openRomFs();
	if (err == 0) {
		if (m_ctx.project) {
			delete m_ctx.project;
			m_ctx.project = nullptr;
		}
		m_ctx.project = project;
		m_oxfsView = new OxFSModel(project->romFs());
		m_projectExplorer->setModel(m_oxfsView);
		connect(m_ctx.project, SIGNAL(updated(QString)), m_oxfsView, SLOT(updateFile(QString)));
		m_importAction->setEnabled(true);
		m_state.projectPath = projectPath;
	}
	return err;
}

int MainWindow::closeProject() {
	auto err = 0;
	if (m_ctx.project) {
		disconnect(m_ctx.project, SIGNAL(updated(QString)), m_oxfsView, SLOT(updateFile(QString)));

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
	return err;
}

void MainWindow::onExit() {
	writeState();
}


// private slots

int MainWindow::openProject() {
	auto projectPath = QFileDialog::getExistingDirectory(this, tr("Select Project Directory..."), QDir::homePath());
	int err = 0;
	if (projectPath != "") {
		err |= openProject(projectPath);
		if (err == 0) {
			err |= writeState();
		}
	}
	return err;
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
				if (QDir(projectPath).exists()) {
					auto path = projectPath + "/" + projectName;
					if (!QDir(path).exists()) {
						Project(path).create();
						openProject(path);
						return 0;
					} else {
						return 1;
					}
				} else {
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
}
