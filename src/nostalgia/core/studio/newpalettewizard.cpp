/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QBuffer>
#include <QDebug>
#include <QFile>

#include <nostalgia/core/consts.hpp>
#include <nostalgia/core/gfx.hpp>

#include "consts.hpp"
#include "newpalettewizard.hpp"

namespace nostalgia::core {

NewPaletteWizardPage::NewPaletteWizardPage(const studio::Context *ctx) {
	m_ctx = ctx;
	addLineEdit(tr("&Palette Name:"), QString(PaletteName) + "*", "", [](QString) {
		return 0;
	});
}

int NewPaletteWizardPage::accept() {
	const auto paletteName = field(PaletteName).toString();
	const auto path = PaletteDir + paletteName + FileExt_npal;
	if (m_ctx->project->exists(path)) {
		showValidationError(tr("A palette with this name already exists."));
		return 1;
	}
	NostalgiaPalette pal;
	m_ctx->project->writeObj(path, &pal);
	return 0;
}

}
