/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <nostalgia/studio/studio.hpp>

namespace nostalgia::core {

class ImportTilesheetWizardPage: public studio::WizardFormPage {
	private:
		static const QString TileSheetDir;
		static const QString TileSheetName;
		static const QString ImportPath;
		static const QString BPP;
		const studio::Context *m_ctx = nullptr;

	public:
		ImportTilesheetWizardPage(const studio::Context *args);

		int accept();

	private:
		int importImage(QFile &srcFile, QString dest);
};

}
