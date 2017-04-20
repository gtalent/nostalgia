/*
 * Copyright 2016-2017 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "newwizard.hpp"

namespace nostalgia {
namespace studio {

using namespace std;

WizardSelect::WizardSelect() {
	m_listWidget = new QListWidget(this);
	auto layout = new QVBoxLayout(this);
	layout->addWidget(m_listWidget);
	setLayout(layout);

	connect(m_listWidget, &QListWidget::activated, this, &WizardSelect::itemSelected);
	connect(m_listWidget, &QListWidget::clicked, this, &WizardSelect::itemSelected);
}

void WizardSelect::initializePage() {
	m_nextId = -1;
	emit completeChanged();
}

void WizardSelect::addOption(QString name, std::function<QWizardPage*()> makePage) {
	m_options[name] = makePage;
	m_listWidget->addItem(name);
}

bool WizardSelect::isComplete() const {
	return m_complete;
}

bool WizardSelect::isFinalPage() const {
	return false;
}

int WizardSelect::nextId() const {
	return m_nextId;
}

void WizardSelect::itemSelected(const QModelIndex &idx) {
	m_complete = true;
	auto w = wizard();

	if (nextId() > -1) {
		w->removePage(nextId());
		m_nextId = -1;
	}

	auto selected = m_listWidget->currentItem()->text();
	if (m_options.contains(selected)) {
		m_nextId = w->addPage(m_options[selected]());
		// for some reason the continue button only appears correctly after remove runs
		w->removePage(nextId());
		m_nextId = w->addPage(m_options[selected]());
	}
}



Wizard::FormPage::FormPage() {
	m_layout = new QGridLayout(this);
	m_layout->setColumnMinimumWidth(0, 20);
	this->setLayout(m_layout);
}

Wizard::FormPage::~FormPage() {
	for (auto l : m_subLayout) {
		delete l;
	}
}

void Wizard::FormPage::addLineEdit(QString displayName, QString fieldName) {
	auto lbl = new QLabel(displayName, this);
	auto le = new QLineEdit(this);
	lbl->setBuddy(le);

	m_layout->addWidget(lbl, currentLine, 0);
	m_layout->addWidget(le, currentLine, 1);

	registerField(fieldName, le);

	currentLine++;
}

void Wizard::FormPage::addFileBrowse(QString displayName, QString fieldName, QString defaultVal) {
	auto layout = new QHBoxLayout();
	auto lbl = new QLabel(displayName, this);
	auto le = new QLineEdit(this);
	auto btn = new QPushButton("Browse...", this);
	lbl->setBuddy(le);

	layout->addWidget(le);
	layout->addWidget(btn);
	m_layout->addWidget(lbl, currentLine, 0);
	m_layout->addLayout(layout, currentLine, 1);

	m_subLayout.push_back(layout);
	registerField(fieldName, le);

	currentLine++;
}


Wizard::Wizard(QWidget *parent): QWizard(parent) {
	setWindowTitle(tr("New..."));
}

}
}
