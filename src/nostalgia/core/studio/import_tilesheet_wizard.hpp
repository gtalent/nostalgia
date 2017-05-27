/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <nostalgia/studio/studio.hpp>

namespace nostalgia {
namespace core {

class ImportTilesheetWizardPage: public studio::WizardFormPage {
	private:
		static const QString TILESHEET_NAME;
		static const QString IMPORT_PATH;
		static const QString BPP;
		studio::Project *m_project = nullptr;

	public:
		ImportTilesheetWizardPage(studio::PluginArgs args);

		int accept();

	private:
		int importImage(QFile &srcFile, QString dest);
};

}
}