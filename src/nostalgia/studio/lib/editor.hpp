/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
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
		bool m_cutEnabled = false;
		bool m_copyEnabled = false;
		bool m_pasteEnabled = false;

	public:
		Editor(QWidget *parent);

		virtual ~Editor() = default;

		/**
		 * Returns the name of item being edited.
		 */
		virtual QString itemName() const = 0;

		virtual void cut();

		virtual void copy();

		virtual void paste();

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
		[[nodiscard]] QUndoStack *undoStack();

		void setExportable(bool);

		[[nodiscard]] bool exportable() const;

		void setCutEnabled(bool);

		[[nodiscard]] bool cutEnabled() const;

		void setCopyEnabled(bool);

		[[nodiscard]] bool copyEnabled() const;

		void setPasteEnabled(bool);

		[[nodiscard]] bool pasteEnabled() const;

	protected:
		/**
		 * Save changes to item being edited.
		 */
		virtual void saveItem();

	signals:
		void unsavedChangesChanged(bool);

		void exportableChanged(bool);

		void cutEnabledChanged(bool);

		void copyEnabledChanged(bool);

		void pasteEnabledChanged(bool);

};

}
