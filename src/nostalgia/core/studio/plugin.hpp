/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QPluginLoader>

#include <nostalgia/studio/studio.hpp>

namespace nostalgia::core {

class Plugin: public QObject, studio::Plugin {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "net.drinkingtea.nostalgia.studio.Plugin" FILE "core-studio.json")
	Q_INTERFACES(nostalgia::studio::Plugin)

	public:
		Plugin();

		QVector<studio::WizardMaker> newWizards(const studio::Context *ctx) override;

		QVector<studio::WizardMaker> importWizards(const studio::Context *args) override;
};

}
