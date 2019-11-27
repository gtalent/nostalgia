/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "plugin.hpp"

namespace nostalgia::studio {

QVector<WizardMaker> Plugin::newWizards(const Context*) {
	return {};
}

QVector<WizardMaker> Plugin::importWizards(const Context*) {
	return {};
}

QWidget *Plugin::makeEditor(QString, const Context*) {
	return nullptr;
}

QVector<EditorMaker> Plugin::editors(const Context*) {
	return {};
}

}
