/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QBuffer>
#include <QFile>

#include <nostalgia/core/consts.hpp>
#include <nostalgia/core/gfx.hpp>

#include "consts.hpp"
#include "new_tilesheet_wizard.hpp"

namespace nostalgia::core {

NewTilesheetWizardPage::NewTilesheetWizardPage(const studio::Context *ctx) {
	m_ctx = ctx;
	addLineEdit(tr("&Tile Sheet Name:"), QString(TileSheetName) + "*", "", [](QString) {
		return 0;
	});
	m_palettePicker = addComboBox(tr("&Palette:"), QString(Palette) + "*", {""});
	m_ctx->project->subscribe(studio::ProjectEvent::FileRecognized, m_palettePicker, [this](QString path) {
		if (path.startsWith(PaletteDir) && path.endsWith(FileExt_npal)) {
			m_palettePicker->addItem(studio::filePathToName(path, PaletteDir, FileExt_npal), path);
		}
	});
}

int NewTilesheetWizardPage::accept() {
	const auto tilesheetName = field(TileSheetName).toString();
	const auto palette = m_palettePicker->itemData(field(Palette).toInt()).toString();
	const auto outPath = QString(TileSheetDir) + tilesheetName + FileExt_ng;
	auto err = m_ctx->project->exists(outPath);
	if (err) {
		showValidationError(tr("A tile sheet with this name already exists."));
		return err;
	}
	NostalgiaGraphic ng;
	ng.columns = 1;
	ng.rows = 1;
	ng.defaultPalette = palette.toUtf8().data();
	m_ctx->project->writeObj(outPath, &ng);
	return 0;
}

}
