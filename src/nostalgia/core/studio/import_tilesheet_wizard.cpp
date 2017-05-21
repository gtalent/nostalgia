/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QFile>

#include "import_tilesheet_wizard.hpp"

namespace nostalgia {
namespace core {

const QString ImportTilesheetWizardPage::TILESHEET_NAME = "projectName";
const QString ImportTilesheetWizardPage::IMPORT_PATH = "projectPath";
const QString ImportTilesheetWizardPage::BPP = "bpp";

ImportTilesheetWizardPage::ImportTilesheetWizardPage(studio::PluginArgs args) {
	m_project = args.project;
	addLineEdit(tr("&Tile Sheet Name:"), TILESHEET_NAME + "*", "", [this](QString) {
			auto importPath = field(IMPORT_PATH).toString();
			if (QFile(importPath).exists()) {
				return 0;
			} else {
				this->showValidationError(tr("Invalid image file: %1").arg(importPath));
				return 1;
			}
		}
	);
	auto fileTypes = "(*.png);;(*.bmp);;(*.jpg);;(*.jpeg)";
	addPathBrowse(tr("Tile Sheet &Path:"), IMPORT_PATH + "*", "",
	              QFileDialog::ExistingFile, fileTypes);
	addComboBox(tr("Bits Per Pixe&l:"), BPP, {"4", "8"});
}

int ImportTilesheetWizardPage::accept() {
	auto tilesheetName = field(TILESHEET_NAME).toString();
	auto importPath = field(IMPORT_PATH).toString();
	QFile importFile(importPath);
	if (importFile.exists()) {
		return importImage(importFile, field(TILESHEET_NAME).toString());
	} else {
		return 1;
	}
}

int ImportTilesheetWizardPage::importImage(QFile &src, QString tilesetName) {
	return 1;
}

}
}
