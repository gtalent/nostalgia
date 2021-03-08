/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
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

class Module {

	public:
		virtual ~Module() = default;

		virtual QVector<WizardMaker> newWizards(const class Context *ctx);

		virtual QVector<WizardMaker> importWizards(const Context *ctx);

	virtual QVector<EditorMaker> editors(const class Context *ctx);

};

}

#define PluginInterface_iid "net.drinkingtea.nostalgia.studio.Module"

Q_DECLARE_INTERFACE(nostalgia::studio::Module, PluginInterface_iid)
