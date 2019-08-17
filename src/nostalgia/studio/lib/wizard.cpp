/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "plugin.hpp"
#include "wizard.hpp"

namespace nostalgia::studio {

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

void WizardSelect::addOption(WizardMaker wm) {
	m_options[wm.name] = {wm.make, wm.onAccept};
	m_listWidget->addItem(wm.name);
}

bool WizardSelect::isComplete() const {
	return m_complete;
}

void WizardSelect::itemSelected(int row) {
	auto w = dynamic_cast<Wizard*>(wizard());
	if (w and row > -1) {
		// remove other pages
		while (nextId() > -1) {
			w->removePage(nextId());
		}

		auto selected = m_listWidget->currentItem()->text();
		if (m_options.contains(selected)) {
			auto &o = m_options[selected];
			for (auto p : o.make()) {
				w->addPage(p);
			}
			w->setAccept(o.onAccept);

			// for some reason the continue button only appears correctly after remove runs
			w->removePage(w->addPage(new QWizardPage()));
		}
	}
}


WizardConclusionPage::WizardConclusionPage(QString msg, QVector<QString> fields) {
	m_baseMsg = msg;
	m_fields = fields;
	setLayout(new QVBoxLayout(this));
}

WizardConclusionPage::~WizardConclusionPage() {
}

void WizardConclusionPage::initializePage() {
	QString msg = m_baseMsg;
	for (auto field : m_fields) {
		msg = msg.arg(this->field(field).toString());
	}

	auto text = new QLabel(msg, this);
	if (m_text) {
		layout()->replaceWidget(m_text, text);
		delete m_text;
	} else {
		layout()->addWidget(text);
	}
	m_text = text;
}


void WizardFormPage::Field::setDisplayText(QString text) {
	auto le = dynamic_cast<QLineEdit*>(this->valueControl);
	auto cb = dynamic_cast<QComboBox*>(this->valueControl);
	if (le) {
		le->setText(text);
	} else if (cb) {
		cb->setCurrentText(text);
	}
}

QString WizardFormPage::Field::getDisplayText() {
	auto le = dynamic_cast<QLineEdit*>(this->valueControl);
	auto cb = dynamic_cast<QComboBox*>(this->valueControl);
	if (le) {
		return le->text();
	} else if (cb) {
		return cb->currentText();
	} else {
		return "";
	}
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

int WizardFormPage::accept() {
	return 0;
}

void WizardFormPage::initializePage() {
	for (auto it = m_fields.begin(); it != m_fields.end(); it++) {
		auto key = it.key();
		auto defaultVal = it.value().defaultValue;
		m_fields[key].setDisplayText(defaultVal);
	}
}

bool WizardFormPage::validatePage() {
	bool retval = true;

	// check validators
	for (auto f : m_fields) {
		if (f.validator != nullptr) {
			if (f.validator(f.getDisplayText()) != 0) {
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

void WizardFormPage::addComboBox(QString displayName, QString fieldName, QVector<QString> options) {
	auto lbl = new QLabel(displayName, this);
	auto cb = new QComboBox(this);
	lbl->setBuddy(cb);

	m_layout->addWidget(lbl, m_currentLine, 0);
	m_layout->addWidget(cb, m_currentLine, 1);

	auto field = &m_fields[fieldName];

	field->valueControl = cb;

	registerField(fieldName, cb);

	for (auto o : options) {
		cb->addItem(o);
	}

	connect(cb, &QComboBox::currentTextChanged, [this, fieldName, field](QString txt) {
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

void WizardFormPage::addLineEdit(QString displayName, QString fieldName, QString defaultVal, function<int(QString)> validator) {
	auto lbl = new QLabel(displayName, this);
	auto le = new QLineEdit(this);
	lbl->setBuddy(le);

	m_layout->addWidget(lbl, m_currentLine, 0);
	m_layout->addWidget(le, m_currentLine, 1);

	auto field = &m_fields[fieldName];

	field->defaultValue = defaultVal;
	field->valueControl = le;
	field->validator = validator;

	registerField(fieldName, le);

	connect(le, &QLineEdit::textChanged, [this, fieldName, field](QString txt) {
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

void WizardFormPage::addPathBrowse(QString displayName, QString fieldName, QString defaultVal,
                                   QFileDialog::FileMode fileMode, QString fileExtensions) {
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
	m_fields[fieldName].valueControl = le;
	m_fields[fieldName].validator = [this, fileMode](QString path) {
		if (fileMode == QFileDialog::Directory) {
			if (!QDir(path).exists()) {
				showValidationError(tr("Specified directory path does not exist."));
				return 1;
			} else {
				return 0;
			}
		} else if (fileMode == QFileDialog::ExistingFile) {
			if (!QFile(path).exists()) {
				showValidationError(tr("Specified directory path does not exist."));
				return 1;
			} else {
				return 0;
			}
		} else {
			return 2;
		}
	};

	registerField(fieldName, le);

	connect(le, &QLineEdit::textChanged, [this, fieldName](QString txt) {
			if (m_fields[fieldName].value == "" && txt != "") {
				m_validFields++;
			} else if (m_fields[fieldName].value != "" && txt == "") {
				m_validFields--;
			}
			m_fields[fieldName].value = txt;
			emit completeChanged();
		}
	);

	connect(btn, &QPushButton::clicked, [this, defaultVal, le, fileMode, fileExtensions]() {
			auto dir = defaultVal;
			if (dir == "") {
				dir = QDir::homePath();
			}
			if (fileMode == QFileDialog::Directory) {
				auto p = QFileDialog::getExistingDirectory(this, tr("Select Directory..."), dir);
				if (p != "") {
					le->setText(p);
				}
			} else if (fileMode == QFileDialog::ExistingFile) {
				auto p = QFileDialog::getOpenFileName(this, tr("Select File..."), dir, fileExtensions);
				if (p != "") {
					le->setText(p);
				}
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
		m_errorMsg->setText(msg);
	} else {
		m_errorMsg->setText("");
	}
}


Wizard::Wizard(QString windowTitle, QWidget *parent): QWizard(parent) {
	setWindowTitle(windowTitle);
	setModal(true);
}

void Wizard::setAccept(std::function<int(QWizard*)> acceptFunc) {
	m_acceptFunc = acceptFunc;
}

void Wizard::accept() {
	auto page = dynamic_cast<WizardFormPage*>(currentPage());
	if ((page == nullptr || page->accept() == 0) &&
	    m_acceptFunc != nullptr && m_acceptFunc(this) == 0) {
		QDialog::accept();
	}
}

}
