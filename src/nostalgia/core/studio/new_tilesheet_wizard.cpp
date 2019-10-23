/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QBuffer>
#include <QFile>

#include "new_tilesheet_wizard.hpp"

namespace nostalgia::core {

NewTilesheetWizardPage::NewTilesheetWizardPage(const studio::Context *ctx) {
	m_ctx = ctx;
	addLineEdit(tr("&Tile Sheet Name:"), QString(TileSheetName) + "*", "", [](QString) {
			return 0;
		}
	);
}

int NewTilesheetWizardPage::accept() {
	auto tilesheetName = field(TileSheetName).toString();
	return 0;
}

}
