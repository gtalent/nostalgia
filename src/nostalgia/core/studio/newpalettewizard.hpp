/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <nostalgia/studio/studio.hpp>

namespace nostalgia::core {

class NewPaletteWizardPage: public studio::WizardFormPage {
	private:
		static constexpr auto PaletteName = "paletteName";
		const studio::Context *m_ctx = nullptr;

	public:
		NewPaletteWizardPage(const studio::Context *args);

		int accept();

	private:
};

}
