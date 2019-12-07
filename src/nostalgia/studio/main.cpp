/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QApplication>
#include <ox/clargs/clargs.hpp>
#include "mainwindow.hpp"

using namespace nostalgia::studio;

int main(int argc, char **args) {
	ox::ClArgs clargs(argc, const_cast<const char**>(args));
	QString argProfilePath = clargs.getString("profile").c_str();

	NostalgiaStudioProfile config;

	QApplication app(argc, args);
	app.setAttribute(Qt::AA_UseHighDpiPixmaps);

	MainWindow w(argProfilePath);
	app.setApplicationName(w.windowTitle());
	w.show();
	QObject::connect(&app, &QApplication::aboutToQuit, &w, &MainWindow::onExit);

	return app.exec();
}
