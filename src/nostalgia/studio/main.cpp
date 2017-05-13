/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <ox/clargs/clargs.hpp>
#include "lib/json.hpp"
#include "mainwindow.hpp"

using namespace nostalgia::studio;
using namespace ox;

int run(int argc, char **args) {
	ClArgs clargs(argc, (const char**) args);
	QString argProfilePath = clargs.getString("profile").c_str();

	NostalgiaStudioProfile config;

	// load in config file
	QFile file(argProfilePath);
	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		QTextStream in(&file);
		read(in.readAll(), &config);
	}

	QApplication app(argc, args);
	app.setApplicationName(config.appName);

	MainWindow w(config);
	w.show();

	return app.exec();
}

int main(int argc, char **args) {
	return run(argc, args);
}
