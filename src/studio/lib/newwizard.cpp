/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "newwizard.hpp"

namespace nostalgia {
namespace studio {

using std::function;

WizardSelect::WizardSelect() {
	m_listWidget = new QListWidget(this);
	auto layout = new QVBoxLayout(this);
	layout->addWidget(m_listWidget);
	setLayout(layout);

	connect(m_listWidget, &QListWidget::currentRowChanged, this, &WizardSelect::itemSelected);
	connect(m_listWidget, &QListWidget::itemSelectionChanged, [this]() {
			m_complete = true;
			emit completeChanged();
		}
	);
	connect(m_listWidget, &QListWidget::doubleClicked, [this]() {
			wizard()->next();
		}
	);
}

void WizardSelect::initializePage() {
	emit completeChanged();
}

void WizardSelect::addOption(QString name, function<QVector<QWizardPage*>()> makePage) {
	m_options[name] = makePage;
	m_listWidget->addItem(name);
}

bool WizardSelect::isComplete() const {
	return m_complete;
}

void WizardSelect::itemSelected(int row) {
	if (row > -1) {
		auto w = wizard();

		if (nextId() > -1) {
			w->removePage(nextId());
		}

		auto selected = m_listWidget->currentItem()->text();
		if (m_options.contains(selected)) {
			for (auto p : m_options[selected]()) {
				w->addPage(p);
			}
			// for some reason the continue button only appears correctly after remove runs
			w->removePage(w->addPage(new QWizardPage()));
		}
	}
}


WizardConclusionPage::WizardConclusionPage(QString msg, QVector<QString> fields) {
	m_baseMsg = msg;
	m_fields = fields;
}

WizardConclusionPage::~WizardConclusionPage() {
}

void WizardConclusionPage::initializePage() {
	QString msg = m_baseMsg;
	for (auto field : m_fields) {
		msg = msg.arg(this->field(field).toString());
	}
	auto text = new QLabel(msg, this);
	auto layout = new QVBoxLayout(this);
	layout->addWidget(text);
	setLayout(layout);
}


WizardFormPage::WizardFormPage() {
	m_layout = new QGridLayout(this);
	m_layout->setColumnMinimumWidth(0, 20);
	this->setLayout(m_layout);
}

WizardFormPage::~WizardFormPage() {
	for (auto l : m_subLayout) {
		delete l;
	}
}

void WizardFormPage::initializePage() {
	for (auto it = m_fields.begin(); it != m_fields.end(); it++) {
		auto key = it.key();
		auto le = m_fields[key].lineEdit;
		auto defaultVal = it.value().defaultValue;
		if (le) {
			le->setText(defaultVal);
		}
	}
}

bool WizardFormPage::validatePage() {
	bool retval = true;

	// check validators
	for (auto f : m_fields) {
		if (f.validator != nullptr) {
			if (f.validator(f.lineEdit->text()) != 0) {
				retval = false;
				break;
			}
		}
	}

	// clear error
	if (retval) {
		showValidationError("");
	}

	return retval;
}

void WizardFormPage::addLineEdit(QString displayName, QString fieldName, QString defaultVal, function<int(QString)> validator) {
	auto lbl = new QLabel(displayName, this);
	auto le = new QLineEdit(this);
	lbl->setBuddy(le);

	m_layout->addWidget(lbl, m_currentLine, 0);
	m_layout->addWidget(le, m_currentLine, 1);

	auto field = &m_fields[fieldName];

	field->defaultValue = defaultVal;
	field->lineEdit = le;
	field->validator = validator;

	registerField(fieldName, le);

	connect(le, &QLineEdit::textChanged, [this, fieldName, le, field](QString txt) {
			if (field->value == "" && txt != "") {
				m_validFields++;
			} else if (field->value != "" && txt == "") {
				m_validFields--;
			}
			field->value = txt;
			emit completeChanged();
		}
	);

	m_currentLine++;
}

void WizardFormPage::addDirBrowse(QString displayName, QString fieldName, QString defaultVal) {
	auto layout = new QHBoxLayout();
	auto lbl = new QLabel(displayName, this);
	auto le = new QLineEdit("", this);
	auto btn = new QPushButton("Browse...", this);
	lbl->setBuddy(le);

	layout->addWidget(le);
	layout->addWidget(btn);
	m_layout->addWidget(lbl, m_currentLine, 0);
	m_layout->addLayout(layout, m_currentLine, 1);

	m_subLayout.push_back(layout);
	m_fields[fieldName].defaultValue = defaultVal;
	m_fields[fieldName].lineEdit = le;
	m_fields[fieldName].validator = [this](QString path) {
		if (!QDir(path).exists()) {
			showValidationError(tr("Specified Project Path directory does not exist."));
			return 1;
		} else {
			return 0;
		}
	};

	registerField(fieldName, le);

	connect(le, &QLineEdit::textChanged, [this, fieldName, le](QString txt) {
			if (m_fields[fieldName].value == "" && txt != "") {
				m_validFields++;
			} else if (m_fields[fieldName].value != "" && txt == "") {
				m_validFields--;
			}
			m_fields[fieldName].value = txt;
			emit completeChanged();
		}
	);

	connect(btn, &QPushButton::clicked, [this, defaultVal, le]() {
			auto p = QFileDialog::getExistingDirectory(this, tr("Select Directory..."), defaultVal);
			if (p != "") {
				le->setText(p);
			}
		}
	);

	m_currentLine++;
}

void WizardFormPage::showValidationError(QString msg) {
	// create label if it is null
	if (!m_errorMsg) {
		m_errorMsg = new QLabel(this);
		m_layout->addWidget(m_errorMsg, m_currentLine, 0, m_currentLine, 2);

		// set text color
		auto pal = m_errorMsg->palette();
		pal.setColor(m_errorMsg->backgroundRole(), Qt::red);
		pal.setColor(m_errorMsg->foregroundRole(), Qt::red);
		m_errorMsg->setPalette(pal);
	}

	// set message
	if (msg != "") {
		m_errorMsg->setText(tr("Error: ") + msg);
	} else {
		m_errorMsg->setText("");
	}
}


Wizard::Wizard(QWidget *parent): QWizard(parent) {
	setWindowTitle(tr("New..."));
	setModal(true);
}

void Wizard::setAccept(std::function<void()> acceptFunc) {
	m_acceptFunc = acceptFunc;
}

void Wizard::accept() {
	m_acceptFunc();
	QDialog::accept();
}

}
}
