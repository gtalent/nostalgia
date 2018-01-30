/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "plugin.hpp"

namespace nostalgia {
namespace studio {

QVector<WizardMaker> Plugin::newWizards(const Context *ctx) {
	return {};
}

QVector<WizardMaker> Plugin::importWizards(const Context *ctx) {
	return {};
}

QWidget *Plugin::makeEditor(QString path, const Context *ctx) {
	return nullptr;
}

}
}
