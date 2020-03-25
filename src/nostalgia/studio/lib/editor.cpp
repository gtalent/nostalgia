/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "editor.hpp"

namespace nostalgia::studio {

Editor::Editor(QWidget *parent): QWidget(parent) {
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

[[nodiscard]] bool Editor::unsavedChanges() noexcept {
	return m_unsavedChanges;
}

QUndoStack *Editor::undoStack() {
	return &m_cmdStack;
}

void Editor::setExportable(bool exportable) {
	m_exportable = exportable;
	emit exportableChanged(exportable);
}

bool Editor::exportable() {
	return m_exportable;
}

void Editor::saveItem() {
}

}
