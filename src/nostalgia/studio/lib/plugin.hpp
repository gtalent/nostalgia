/*
 * Copyright 2016 - 2018 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <functional>

#include <QMainWindow>
#include <QPointer>
#include <QSharedPointer>
#include <QVector>
#include <QWizardPage>

#include "project.hpp"
#include "wizard.hpp"

namespace nostalgia {
namespace studio {

struct Context {
	QWidget *tabParent = nullptr;
	const Project *project = nullptr;
};

struct EditorMaker {
	virtual ~EditorMaker() = default;

	virtual QWidget *make(QString path, const Context *ctx) = 0;
};

class Plugin {

	public:
		virtual ~Plugin() = default;

		virtual QVector<WizardMaker> newWizards(const Context *ctx);

		virtual QVector<WizardMaker> importWizards(const Context *ctx);

		virtual QWidget *makeEditor(QString path, const Context *ctx);

};

}
}

#define PluginInterface_iid "net.drinkingtea.nostalgia.studio.Plugin"

Q_DECLARE_INTERFACE(nostalgia::studio::Plugin, PluginInterface_iid)
