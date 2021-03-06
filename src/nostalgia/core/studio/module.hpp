/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QPluginLoader>

#include <nostalgia/studio/studio.hpp>

namespace nostalgia::core {

class Module: public QObject, public studio::Module {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "net.drinkingtea.nostalgia.core.studio.Module" FILE "core-studio.json")
	Q_INTERFACES(nostalgia::studio::Module)

	public:
		QVector<studio::WizardMaker> newWizards(const studio::Context *ctx) override;

		QVector<studio::WizardMaker> importWizards(const studio::Context *args) override;

		QVector<studio::EditorMaker> editors(const studio::Context *ctx) override;

};

}
