/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <nostalgia/studio/studio.hpp>

#include "consts.hpp"

namespace nostalgia::core {

constexpr auto TileSheetName = "tilesheetName";
constexpr auto ImportPath = "importPath";
constexpr auto Palette = "palette";
constexpr auto PaletteName = "paletteName";

class ImportTilesheetWizardMainPage: public studio::WizardFormPage {
	private:
		//static constexpr auto BPP = "bpp";
		const studio::Context *m_ctx = nullptr;

	public:
		ImportTilesheetWizardMainPage(const studio::Context *args);
};

class ImportTilesheetWizardPalettePage: public studio::WizardFormPage {
	private:
		//static constexpr auto BPP = "bpp";
		const studio::Context *m_ctx = nullptr;

	public:
		ImportTilesheetWizardPalettePage(const studio::Context *args);

		int accept();
};

}
