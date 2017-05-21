/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "plugin.hpp"

namespace nostalgia {
namespace studio {

void Plugin::addNewWizard(QString name, std::function<QVector<QWizardPage*>()> make) {
	m_newWizards.push_back({name, make});
}

QVector<WizardMaker> Plugin::newWizards(PluginArgs) {
	return {};
}

QVector<WizardMaker> Plugin::importWizards(PluginArgs) {
	return {};
}

}
}
