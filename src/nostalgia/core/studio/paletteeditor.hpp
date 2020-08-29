/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <nostalgia/core/gfx.hpp>
#include <nostalgia/studio/studio.hpp>

namespace nostalgia::core {

class PaletteEditor: public studio::Editor {

	friend class AddColorCommand;
	friend class RemoveColorCommand;
	friend class UpdateColorCommand;
	friend class MoveColorCommand;

	private:
		const studio::Context *m_ctx = nullptr;
		QString m_itemPath;
		std::unique_ptr<NostalgiaPalette> m_pal;
		class QTableWidget *m_table = nullptr;
		class QPushButton *m_addBtn = nullptr;
		class QPushButton *m_rmBtn = nullptr;
		class QPushButton *m_moveUpBtn = nullptr;
		class QPushButton *m_moveDownBtn = nullptr;

	public:
		PaletteEditor(QString path, const studio::Context *ctx, QWidget *parent);

		/**
		 * Returns the name of item being edited.
		 */
		QString itemName() const override;

	protected:
		void addColor(int idx, Color16 c);

		void rmColor(int idx);

		void updateColor(int idx, Color16);

		void moveColor(int idx, int offset);

		void saveItem() override;

	private:
		void load();

		[[nodiscard]] Color16 rowColor(int i) const;

		void addTableRow(int i, Color16 c);

		void rmTableRow(int i);

		void setTableRow(int idx, Color16 c);

	private slots:
		void colorSelected();

		void cellChanged(int row, int col);

		void addColorClicked();

		void rmColorClicked();

		void moveColorUpClicked();

		void moveColorDownClicked();

};

}
