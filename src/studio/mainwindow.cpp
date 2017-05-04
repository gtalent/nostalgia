/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QVector>
#include "lib/newwizard.hpp"
#include "lib/project.hpp"
#include "mainwindow.hpp"

namespace nostalgia {
namespace studio {

MainWindow::MainWindow(NostalgiaStudioProfile config, QWidget *parent) {
	auto screenSize = QApplication::desktop()->screenGeometry();

	// set window to 75% of screen width, and center NostalgiaStudioProfile
	auto sizePct = 0.75;
	resize(screenSize.width() * sizePct, screenSize.height() * sizePct);
	move(-x(), -y());
	move(screenSize.width() * (1 - sizePct) / 2, screenSize.height() * (1 - sizePct) / 2);

	setWindowTitle(config.app_name);

	setupMenu();
}

MainWindow::~MainWindow() {
	for (auto f : m_cleanupTasks) {
		f();
	}
}

void MainWindow::setupMenu() {
	auto menu = menuBar();
	auto fileMenu = menu->addMenu(tr("&File"));

	// New...
	addAction(
		fileMenu,
		tr("&New..."),
		tr(""),
		QKeySequence::New,
		this,
		SLOT(showNewWizard())
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

void MainWindow::addAction(QMenu *menu, QString text, QString toolTip,
                           QKeySequence::StandardKey key, const QObject *tgt, const char *cb) {
	auto action = menu->addAction(text);
	action->setShortcuts(key);
	action->setStatusTip(toolTip);
	auto conn = connect(action, SIGNAL(triggered()), tgt, cb);
	m_cleanupTasks.push_back([this, conn]() {
		QObject::disconnect(conn);
	});
}

void MainWindow::addAction(QMenu *menu, QString text, QString toolTip,
                           QKeySequence::StandardKey key, void (*cb)()) {
	auto action = menu->addAction(text);
	action->setShortcuts(key);
	action->setStatusTip(toolTip);
	auto conn = connect(action, &QAction::triggered, cb);
	m_cleanupTasks.push_back([this, conn]() {
		QObject::disconnect(conn);
	});
}

void MainWindow::showNewWizard() {
	const QString PROJECT_NAME = "projectName";
	const QString PROJECT_PATH = "projectPath";
	Wizard wizard;
	auto ws = new WizardSelect();
	wizard.addPage(ws);
	ws->addOption(tr("Project"),
			[&wizard, PROJECT_NAME, PROJECT_PATH]() {
			QVector<QWizardPage*> pgs;
			auto pg = new WizardFormPage();
			pg->addLineEdit(tr("Project &Name:"), PROJECT_NAME + "*", "", [PROJECT_PATH, pg, &wizard](QString projectName) {
					auto projectPath = wizard.field(PROJECT_PATH).toString();
					auto path = projectPath + "/" + projectName;
					if (!QDir(path).exists()) {
						return 0;
					} else {
						pg->showValidationError(tr("This project directory already exists."));
						return 1;
					}
				}
			);
			pg->addDirBrowse(tr("Project &Path:"), PROJECT_PATH + "*");
			pgs.push_back(pg);
			pgs.push_back(new WizardConclusionPage(tr("Creating project: %1/%2"), {PROJECT_PATH, PROJECT_NAME}));

			return pgs;
		}
	);
	wizard.setAccept([&wizard, ws, PROJECT_NAME, PROJECT_PATH]() {
			auto projectName = wizard.field(PROJECT_NAME).toString();
			auto projectPath = wizard.field(PROJECT_PATH).toString();
			if (QDir(projectPath).exists()) {
				auto path = projectPath + "/" + projectName;
				if (!QDir(path).exists()) {
					Project(path).create();
				}
			}
		}
	);
	wizard.show();
	wizard.exec();
}

}
}
