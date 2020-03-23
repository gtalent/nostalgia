/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QBuffer>
#include <QDebug>
#include <QFile>

#include <nostalgia/core/consts.hpp>

#include "imgconv.hpp"
#include "import_tilesheet_wizard.hpp"

namespace nostalgia::core {

ImportTilesheetWizardMainPage::ImportTilesheetWizardMainPage(const studio::Context *ctx) {
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
	addLineEdit(tr("Til&es:"), QString(TileCount), "");
}

ImportTilesheetWizardPalettePage::ImportTilesheetWizardPalettePage(const studio::Context *ctx) {
	m_ctx = ctx;
	addLineEdit(tr("Palette &Name:"), PaletteName);
}

int ImportTilesheetWizardPalettePage::accept() {
	const auto tilesheetName = field(TileSheetName).toString();
	const auto importPath = field(ImportPath).toString();
	const auto tileCount = field(TileCount).toInt();
	const auto paletteName = field(PaletteName).toString();
	const auto outPath = TileSheetDir + tilesheetName + FileExt_ng;
	if (!QFile(importPath).exists()) {
		return OxError(1);
	}
	auto ng = imgToNg(importPath, tileCount, 0);
	if (!ng) {
		return OxError(1);
	}
	const auto paletteOutPath = PaletteDir + paletteName + FileExt_npal;
	core::NostalgiaPalette pal;
	pal = std::move(ng->pal);
	m_ctx->project->writeObj(paletteOutPath, &pal);
	auto defaultPalette = paletteOutPath.toUtf8();
	ng->defaultPalette = defaultPalette.data();
	m_ctx->project->writeObj(outPath, ng.get());
	return 0;
}

}
