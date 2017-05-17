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
#include <QVector>
#include <QWizardPage>

namespace nostalgia {
namespace studio {

struct WizardMaker {
	QString name;
	std::function<QVector<QWizardPage*>()> make;
};

class Plugin {
	private:
		QVector<WizardMaker> m_newWizards;
		QVector<WizardMaker> m_importWizards;

	public:
		void addNewWizard(QString name, std::function<QVector<QWizardPage*>()> make);

		void addImportWizard(QString name, std::function<QVector<QWizardPage*>()> make);

		QVector<WizardMaker> newWizards();

		QVector<WizardMaker> importWizards();
};

}
}

#define PluginInterface_iid "net.drinkingtea.nostalgia.studio.Plugin"

Q_DECLARE_INTERFACE(nostalgia::studio::Plugin, PluginInterface_iid)
