/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QDebug>

#include <nostalgia/world/world.hpp>

#include "consts.hpp"
#include "newworldwizard.hpp"
#include "worldeditor.hpp"

#include "worldstudioplugin.hpp"

namespace nostalgia {
namespace world {

using namespace studio;

QVector<WizardMaker> WorldEditorPlugin::newWizards(const Context *ctx) {
	return {
		{
			tr("Zone"),
			[ctx]() -> QVector<QWizardPage*> {
				return {new NewWorldWizard(ctx)};
			},
			[ctx](QWizard *w) {
				w->field(NewWorldWizard::FIELD_WORLD_PATH).toString();
				w->field(NewWorldWizard::FIELD_WIDTH).toInt();
				w->field(NewWorldWizard::FIELD_HEIGHT).toInt();
				return 0;
			}
		}
	};
}

QWidget *WorldEditorPlugin::makeEditor(QString path, const Context *ctx) {
	if (path.startsWith(PATH_ZONES)) {
		return new WorldEditor(path, ctx);
	} else {
		return nullptr;
	}
}

}
}
