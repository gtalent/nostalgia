/*
 * Copyright 2016-2017 gtalent2@gmail.com
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

namespace nostalgia {
namespace studio {

struct PluginArgs {
	Project *project = nullptr;
};

struct WizardMaker {
	QString name;
	std::function<QVector<QWizardPage*>()> make;
};

class Plugin {
	private:
		QVector<WizardMaker> m_newWizards;

	public:
		void addNewWizard(QString name, std::function<QVector<QWizardPage*>()> make);

		void addImportWizard(QString name, std::function<QVector<QWizardPage*>()> make);

		virtual QVector<WizardMaker> newWizards(PluginArgs);

		virtual QVector<WizardMaker> importWizards(PluginArgs);
};

}
}

#define PluginInterface_iid "net.drinkingtea.nostalgia.studio.Plugin"

Q_DECLARE_INTERFACE(nostalgia::studio::Plugin, PluginInterface_iid)
