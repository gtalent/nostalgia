/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "context.hpp"
#include "module.hpp"

namespace nostalgia::studio {

QVector<WizardMaker> Module::newWizards(const Context*) {
	return {};
}

QVector<WizardMaker> Module::importWizards(const Context*) {
	return {};
}

QVector<EditorMaker> Module::editors(const Context*) {
	return {};
}

}
