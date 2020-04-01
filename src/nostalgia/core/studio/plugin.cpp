/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "new_tilesheet_wizard.hpp"
#include "newpalettewizard.hpp"
#include "import_tilesheet_wizard.hpp"
#include "paletteeditor.hpp"
#include "tilesheeteditor.hpp"

#include "plugin.hpp"

using namespace nostalgia::studio;

namespace nostalgia::core {

QVector<studio::WizardMaker> Plugin::newWizards(const studio::Context *ctx) {
	return {
		{
			tr("Palette"),
			[ctx]() {
				QVector<QWizardPage*> pgs;
				pgs.push_back(new NewPaletteWizardPage(ctx));
				return pgs;
			}
		},
		{
			tr("Tile Sheet"),
			[ctx]() {
				QVector<QWizardPage*> pgs;
				pgs.push_back(new NewTilesheetWizardPage(ctx));
				return pgs;
			}
		},
	};
}

QVector<studio::WizardMaker> Plugin::importWizards(const studio::Context *ctx) {
	return {
		{
			tr("Tile Sheet"),
			[ctx]() {
				QVector<QWizardPage*> pgs;
				pgs.push_back(new ImportTilesheetWizardMainPage(ctx));
				pgs.push_back(new ImportTilesheetWizardPalettePage(ctx));
				return pgs;
			}
		}
	};
}

QVector<studio::EditorMaker> Plugin::editors(const studio::Context *ctx) {
	return {
		{
			{"ng"},
			[ctx](QString path) {
				return new TileSheetEditor(path, ctx, ctx->tabParent);
			}
		},
		{
			{"npal"},
			[ctx](QString path) {
				return new PaletteEditor(path, ctx, ctx->tabParent);
			}
		}
	};
}

}
