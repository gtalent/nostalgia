/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "editor.hpp"

namespace nostalgia::studio {

Editor::Editor(QWidget *parent): QWidget(parent) {
}

void Editor::cut() {
}

void Editor::copy() {
}

void Editor::paste() {
}

void Editor::exportFile() {
}

void Editor::save() {
    saveItem();
    setUnsavedChanges(false);
}

void Editor::setUnsavedChanges(bool uc) {
    m_unsavedChanges = uc;
    emit unsavedChangesChanged(uc);
}

bool Editor::unsavedChanges() noexcept {
	return m_unsavedChanges;
}

QUndoStack *Editor::undoStack() {
	return &m_cmdStack;
}

void Editor::setExportable(bool exportable) {
	m_exportable = exportable;
	emit exportableChanged(exportable);
}

bool Editor::exportable() const {
	return m_exportable;
}

void Editor::setCutEnabled(bool v) {
	m_cutEnabled = v;
	emit cutEnabledChanged(v);
}

bool Editor::cutEnabled() const {
	return m_cutEnabled;
}

void Editor::setCopyEnabled(bool v) {
	m_copyEnabled = v;
	emit copyEnabledChanged(v);
}

bool Editor::copyEnabled() const {
	return m_copyEnabled;
}

void Editor::setPasteEnabled(bool v) {
	m_pasteEnabled = v;
	emit pasteEnabledChanged(v);
}

bool Editor::pasteEnabled() const {
	return m_pasteEnabled;
}

void Editor::saveItem() {
}

}
