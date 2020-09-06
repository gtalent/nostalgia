/*
 * Copyright 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <qdark/theme.hpp>

namespace qdark {

void load(QApplication *app) {
	// load theme
	QFile theme(":qdarkstyle/style.qss");
	theme.open(QFile::ReadOnly | QFile::Text);
	QTextStream themeStream(&theme);
	app->setStyleSheet(themeStream.readAll());
}

}
