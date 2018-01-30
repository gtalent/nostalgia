/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
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
}

}
}
