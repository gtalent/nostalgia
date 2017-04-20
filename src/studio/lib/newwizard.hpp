/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <functional>
#include <QGridLayout>
#include <QListWidget>
#include <QMap>
#include <QVector>
#include <QWizard>

namespace nostalgia {
namespace studio {

class WizardSelect: public QWizardPage {
	Q_OBJECT

	private:
		QMap<QString, std::function<QWizardPage*()>> m_options;
		QListWidget *m_listWidget = nullptr;
		bool m_complete = false;
		int m_nextId = -1;

	public:
		WizardSelect();

		void initializePage();

		void addOption(QString name, std::function<QWizardPage*()> makePage);

		bool isComplete() const;

		bool isFinalPage() const;

		int nextId() const;

	private slots:
		void itemSelected(const QModelIndex &idx);
};


class Wizard: public QWizard {
	Q_OBJECT

	public:
		class FormPage: public QWizardPage {
			private:
				QGridLayout *m_layout = nullptr;
				QVector<QLayout*> m_subLayout;
				int currentLine = 0;

			public:
				FormPage();

				~FormPage();

				void addLineEdit(QString displayName, QString fieldName);

				void addFileBrowse(QString displayName, QString fieldName, QString defaultVal = "");
		};

	public:
		Wizard(QWidget *parent = 0);
};

}
}
