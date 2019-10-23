/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "new_tilesheet_wizard.hpp"
#include "import_tilesheet_wizard.hpp"

#include "plugin.hpp"

using namespace nostalgia::studio;

namespace nostalgia::core {

Plugin::Plugin() {
}

QVector<studio::WizardMaker> Plugin::newWizards(const Context *ctx) {
	return {
		{
			tr("Tile Sheet"),
			[ctx]() {
				QVector<QWizardPage*> pgs;
				pgs.push_back(new NewTilesheetWizardPage(ctx));
				return pgs;
			}
		}
	};
}

QVector<studio::WizardMaker> Plugin::importWizards(const studio::Context *ctx) {
	return {
		{
			tr("Tile Sheet"),
			[ctx]() {
				QVector<QWizardPage*> pgs;
				pgs.push_back(new ImportTilesheetWizardMainPage(ctx));
				pgs.push_back(new ImportTilesheetWizardPalettePage(ctx));
				return pgs;
			}
		}
	};
}

}
