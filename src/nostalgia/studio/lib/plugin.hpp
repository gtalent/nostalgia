/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <functional>

#include <QVector>
#include <QWizardPage>

#include "wizard.hpp"

namespace nostalgia::studio {

struct EditorMaker {
	QStringList fileTypes;
	std::function<class Editor*(QString)> make;
};

class Plugin {

	public:
		virtual ~Plugin() = default;

		virtual QVector<WizardMaker> newWizards(const class Context *ctx);

		virtual QVector<WizardMaker> importWizards(const Context *ctx);

		virtual QWidget *makeEditor(QString path, const class Context *ctx);

		virtual QVector<EditorMaker> editors(const class Context *ctx);

};

}

#define PluginInterface_iid "net.drinkingtea.nostalgia.studio.Plugin"

Q_DECLARE_INTERFACE(nostalgia::studio::Plugin, PluginInterface_iid)
