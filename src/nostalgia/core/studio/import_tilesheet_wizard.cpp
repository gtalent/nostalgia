/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QBuffer>
#include <QFile>

#include "import_tilesheet_wizard.hpp"

namespace nostalgia::core {

ImportTilesheetWizardPage::ImportTilesheetWizardPage(const studio::Context *ctx) {
	m_ctx = ctx;
	addLineEdit(tr("&Tile Sheet Name:"), QString(TileSheetName) + "*", "", [this](QString) {
			auto importPath = field(ImportPath).toString();
			if (QFile(importPath).exists()) {
				return 0;
			} else {
				this->showValidationError(tr("Invalid image file: %1").arg(importPath));
				return 1;
			}
		}
	);
	auto fileTypes = "(*.png);;(*.bmp);;(*.jpg);;(*.jpeg)";
	addPathBrowse(tr("Tile Sheet &Path:"), QString(ImportPath) + "*", "",
	              QFileDialog::ExistingFile, fileTypes);
	//addComboBox(tr("Bits Per Pixe&l:"), BPP, {tr("Auto"), "4", "8"});
}

int ImportTilesheetWizardPage::accept() {
	auto tilesheetName = field(TileSheetName).toString();
	auto importPath = field(ImportPath).toString();
	QFile importFile(importPath);
	if (importFile.exists()) {
		return importImage(importFile, field(TileSheetName).toString());
	} else {
		return 1;
	}
}

int ImportTilesheetWizardPage::importImage(QFile &srcFile, QString tilesheetName) {
	if (srcFile.exists()) {
		srcFile.open(QIODevice::ReadOnly);
		auto buff = srcFile.readAll();
		QImage srcImg;
		if (srcImg.loadFromData(buff)) {
			int err = 0;
			// ensure image is PNG
			QByteArray out;
			QBuffer outBuffer(&out);
			outBuffer.open(QIODevice::WriteOnly);
			srcImg.save(&outBuffer, "PNG");
			// make sure tile sheet directory exists
			m_ctx->project->mkdir(TileSheetDir);
			// write image
			err |= m_ctx->project->write(TileSheetDir + tilesheetName + ".png", reinterpret_cast<uint8_t*>(out.data()), out.size());
			err |= m_ctx->project->saveRomFs();
			return err;
		} else {
			return 1;
		}
	} else {
		return 2;
	}
}

}
