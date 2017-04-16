/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QApplication>
#include <QDesktopWidget>
#include "mainwindow.hpp"

namespace nostalgia {
namespace studio {

MainWindow::MainWindow(NostalgiaStudioProfile config, QWidget *parent) {
	auto screenSize = QApplication::desktop()->screenGeometry();

	// set window to 70% of screen width, and center NostalgiaStudioProfile
	auto sizePct = 0.7;
	resize(screenSize.width() * sizePct, screenSize.height() * sizePct);
	move(-x(), -y());
	move(screenSize.width() * (1 - sizePct) / 2, screenSize.height() * (1 - sizePct) / 2);

	setWindowTitle(config.app_name);
}

MainWindow::~MainWindow() {
}

}
}
