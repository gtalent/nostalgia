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

void Editor::saveItem() {
}

QUndoStack *Editor::undoStack() {
	return nullptr;
}

void Editor::save() {
    save();
    setUnsavedChanges(false);
}

void Editor::setUnsavedChanges(bool uc) {
    m_unsavedChanges = uc;
    emit unsavedChangesUpdate(uc);
}

}
