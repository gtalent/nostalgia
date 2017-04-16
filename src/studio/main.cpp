/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QApplication>
#include "studio/mainwindow.hpp"

using namespace nostalgia::studio;

int main(int argc, char **argv) {
	QApplication app(argc, argv);

	MainWindow w;
	w.show();

	return app.exec();
}
