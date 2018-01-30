/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "import_tilesheet_wizard.hpp"

#include "plugin.hpp"

using namespace nostalgia::studio;

namespace nostalgia {
namespace core {

Plugin::Plugin() {
}

QVector<studio::WizardMaker> Plugin::importWizards(const studio::Context *args) {
	return {
		{
			tr("Tile Sheet"),
			[args]() {
				QVector<QWizardPage*> pgs;
				pgs.push_back(new ImportTilesheetWizardPage(args));
				return pgs;
			}
		}
	};
}

}
}
