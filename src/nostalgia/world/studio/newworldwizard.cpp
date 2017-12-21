/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QDebug>

#include "consts.hpp"
#include "newworldwizard.hpp"

namespace nostalgia {
namespace world {

using namespace studio;

const QString NewWorldWizard::FIELD_WORLD_PATH = "World.WorldPath";
const QString NewWorldWizard::FIELD_WIDTH = "World.Width";
const QString NewWorldWizard::FIELD_HEIGHT = "World.Height";

NewWorldWizard::NewWorldWizard(const Context *ctx) {
	addLineEdit(tr("&Name:"), FIELD_WORLD_PATH, "", [this, ctx](QString worldName) {
			worldName = PATH_ZONES + worldName;
			if (ctx->project->stat(worldName).inode == 0) {
				return 0;
			} else {
				this->showValidationError(tr("World already exists: %1").arg(worldName));
				return 1;
			}
		}
	);
	addLineEdit(tr("&Width:"), FIELD_WIDTH, "", [this, ctx](QString widthStr) {
			bool ok = false;
			widthStr.toInt(&ok);
			if (ok) {
				return 0;
			} else {
				this->showValidationError(tr("Invalid width: \"%1\"").arg(widthStr));
				return 1;
			}
		}
	);
	addLineEdit(tr("&Height:"), FIELD_HEIGHT, "", [this, ctx](QString widthStr) {
			bool ok = false;
			widthStr.toInt(&ok);
			if (ok) {
				return 0;
			} else {
				this->showValidationError(tr("Invalid height: \"%1\"").arg(widthStr));
				return 1;
			}
		}
	);
}

}
}
