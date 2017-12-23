/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QPluginLoader>

#include <nostalgia/studio/studio.hpp>

namespace nostalgia {
namespace world {

class WorldEditorPlugin: public QObject, public studio::Plugin {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "net.drinkingtea.nostalgia.studio.Plugin")
	Q_INTERFACES(nostalgia::studio::Plugin)

	public:
		QVector<studio::WizardMaker> newWizards(const studio::Context *ctx) override;

		QWidget *makeEditor(QString path, const studio::Context *ctx) override;

};

}
}