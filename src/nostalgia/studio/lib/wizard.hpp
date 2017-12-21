/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <functional>

#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QVector>
#include <QWizard>

namespace nostalgia {
namespace studio {

class WizardMaker;

class WizardSelect: public QWizardPage {
	Q_OBJECT

	private:
		struct WizardFuncs {
			std::function<QVector<QWizardPage*>()> make;
			std::function<int(QWizard*)> onAccept;
		};

		QHash<QString, WizardFuncs> m_options;
		QListWidget *m_listWidget = nullptr;
		bool m_complete = false;

	public:
		WizardSelect();

		void addOption(WizardMaker wm);

		void initializePage() override;

		bool isComplete() const override;

	private slots:
		void itemSelected(int row);
};


class WizardFormPage: public QWizardPage {
	Q_OBJECT
	private:
		struct Field {
			QString defaultValue = "";
			QString value = "";
			QWidget *valueControl = nullptr;
			std::function<int(QString)> validator;

			void setDisplayText(QString text);

			QString getDisplayText();
		};
		QLabel *m_errorMsg = nullptr;
		QGridLayout *m_layout = nullptr;
		QVector<QLayout*> m_subLayout;
		QMap<QString, Field> m_fields;
		int m_currentLine = 0;
		int m_validFields = 0;

	public:
		WizardFormPage();

		~WizardFormPage();

		virtual int accept();

		void initializePage() override;

		bool validatePage() override;

		void addComboBox(QString displayName, QString fieldName, QVector<QString> options);

		void addLineEdit(QString displayName, QString fieldName,
                       QString defaultVal = "",
							  std::function<int(QString)> validator = [](QString) { return 0; });

		void addPathBrowse(QString displayName, QString fieldName,
		                   QString defaultVal = QDir::homePath(),
                         QFileDialog::FileMode fileMode = QFileDialog::AnyFile,
		                   QString fileExtensions = "");

		void showValidationError(QString msg);
};


class WizardConclusionPage: public QWizardPage {
	Q_OBJECT
	private:
		QString m_baseMsg = "";
		QLabel *m_text = nullptr;
		QVector<QString> m_fields;

	public:
		WizardConclusionPage(QString msg, QVector<QString> field);

		virtual ~WizardConclusionPage();

		void initializePage() override;
};


class Wizard: public QWizard {
	Q_OBJECT
	private:
		std::function<int(QWizard*)> m_acceptFunc;

	public:
		Wizard(QString windowTitle, QWidget *parent = 0);

		void setAccept(std::function<int(QWizard*)> acceptFunc);

		void accept();
};

}
}
