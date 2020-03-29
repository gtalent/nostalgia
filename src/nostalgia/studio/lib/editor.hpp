/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QUndoStack>
#include <QWidget>

#include "nostalgiastudio_export.h"

namespace nostalgia::studio {

class NOSTALGIASTUDIO_EXPORT Editor: public QWidget {
	Q_OBJECT

	private:
		QUndoStack m_cmdStack;
		bool m_unsavedChanges = false;
		bool m_exportable = false;

	public:
		Editor(QWidget *parent);

		virtual ~Editor() = default;

		/**
		 * Returns the name of item being edited.
		 */
		virtual QString itemName() const = 0;

		virtual void exportFile();

		/**
		 * Save changes to item being edited.
		 */
		void save();

		/**
		 * Sets indication of item being edited has unsaved changes. Also emits
		 * unsavedChangesChanged signal.
		 */
		void setUnsavedChanges(bool);

		[[nodiscard]] bool unsavedChanges() noexcept;

		/**
		 * Returns the undo stack holding changes to the item being edited.
		 */
		QUndoStack *undoStack();

		void setExportable(bool);

		bool exportable() const;

	protected:
		/**
		 * Save changes to item being edited.
		 */
		virtual void saveItem();

	signals:
		void unsavedChangesChanged(bool);

		void exportableChanged(bool);

};

}
