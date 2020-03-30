/*
 * Copyright 2016 - 2020 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QHeaderView>
#include <QItemDelegate>
#include <QPainter>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QToolBar>

#include <nostalgia/core/gfx.hpp>

#include "util.hpp"
#include "paletteeditor.hpp"

namespace nostalgia::core {

enum class PaletteEditorCommandId {
	AddColor,
	RemoveColor,
	UpdateColor,
};

class ColorTableDelegate: public QStyledItemDelegate {
	public:
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex &idx) const {
			const auto max = idx.column() != 3 ? 31 : 1;
			auto le = new QLineEdit(parent);
			auto validator = new QIntValidator(0, max, le);
			le->setValidator(validator);
			return le;
		}

		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
			if (index.column() != 4) {
				if (option.state & QStyle::State_Selected) {
					painter->fillRect(option.rect, option.palette.highlight());
				}
				QStyledItemDelegate::paint(painter, option, index);
			} else {
				auto color = index.model()->data(index, Qt::DisplayRole).toString();
				painter->fillRect(option.rect, QColor(color));
			}
		}
};

class AddColorCommand: public QUndoCommand {
	private:
		PaletteEditor *m_editor = nullptr;
		Color16 m_color = 0;
		int m_idx = -1;

	public:
		AddColorCommand(PaletteEditor *editor, Color16 color, int idx) {
			m_editor = editor;
			m_color = color;
			m_idx = idx;
		}

		virtual ~AddColorCommand() = default;

		int id() const override {
			return static_cast<int>(PaletteEditorCommandId::AddColor);
		}

		void redo() override {
			m_editor->addColor(m_idx, m_color);
		}

		void undo() override {
			m_editor->rmColor(m_idx);
		}

};

class RemoveColorCommand: public QUndoCommand {
	private:
		PaletteEditor *m_editor = nullptr;
		Color16 m_color = 0;
		int m_idx = -1;

	public:
		RemoveColorCommand(PaletteEditor *editor, Color16 color, int idx) {
			m_editor = editor;
			m_color = color;
			m_idx = idx;
		}

		virtual ~RemoveColorCommand() = default;

		int id() const override {
			return static_cast<int>(PaletteEditorCommandId::RemoveColor);
		}

		void redo() override {
			m_editor->rmColor(m_idx);
		}

		void undo() override {
			m_editor->addColor(m_idx, m_color);
		}

};

class UpdateColorCommand: public QUndoCommand {
	private:
		PaletteEditor *m_editor = nullptr;
		Color16 m_oldColor = 0;
		Color16 m_newColor = 0;
		int m_idx = -1;

	public:
		UpdateColorCommand(PaletteEditor *editor, int idx, Color16 oldColor, Color16 newColor) {
			m_editor = editor;
			m_idx = idx;
			m_oldColor = oldColor;
			m_newColor = newColor;
			setObsolete(m_oldColor == m_newColor);
		}

		virtual ~UpdateColorCommand() = default;

		int id() const override {
			return static_cast<int>(PaletteEditorCommandId::UpdateColor);
		}

		void redo() override {
			m_editor->updateColor(m_idx, m_newColor);
		}

		void undo() override {
			m_editor->updateColor(m_idx, m_oldColor);
		}

};


static QTableWidgetItem *mkCell(QString v, bool editable = true) {
	auto c = new QTableWidgetItem;
	c->setText(v);
	c->setFont(QFont("monospace"));
	c->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
	if (!editable) {
		c->setFlags(c->flags() & ~Qt::ItemIsEditable);
	}
	return c;
}


static QTableWidgetItem *mkCell(uint8_t v) {
	return mkCell(QString("%1").arg(v), true);
}


PaletteEditor::PaletteEditor(QString path, const studio::Context *ctx, QWidget *parent): studio::Editor(parent) {
	m_ctx = ctx;
	m_itemPath = path;

	auto canvasLyt = new QVBoxLayout(this);
	auto tb = new QToolBar(tr("Tile Sheet Options"));
	auto addBtn = new QPushButton(tr("Add"), tb);
	m_rmBtn = new QPushButton(tr("Remove"), tb);
	m_rmBtn->setEnabled(false);
	tb->addWidget(addBtn);
	tb->addWidget(m_rmBtn);
	canvasLyt->setMenuBar(tb);

	m_table = new QTableWidget(this);
	m_table->setItemDelegate(new ColorTableDelegate);
	m_table->setColumnCount(5);
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->setHorizontalHeaderLabels(QStringList() << tr("Red") << tr("Green") << tr("Blue") << tr("Alpha") << tr("Color Preview"));
	m_table->horizontalHeader()->setStretchLastSection(true);
	m_table->verticalHeader()->hide();
	canvasLyt->addWidget(m_table);
	connect(m_table, &QTableWidget::itemSelectionChanged, this, &PaletteEditor::colorSelected);
	connect(m_rmBtn, &QPushButton::clicked, this, &PaletteEditor::rmColorClicked);
	connect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
	m_pal = m_ctx->project->loadObj<NostalgiaPalette>(m_itemPath);
	load();
}

QString PaletteEditor::itemName() const {
	return m_itemPath.mid(m_itemPath.lastIndexOf('/'));
}

void PaletteEditor::addColor(int idx, Color16 c) {
	addTableRow(idx, c);
	m_pal->colors.insert(idx, c);
	setUnsavedChanges(true);
}

void PaletteEditor::rmColor(int idx) {
	rmTableRow(idx);
	m_pal->colors.erase(idx);
	setUnsavedChanges(true);
}

void PaletteEditor::addTableRow(int i, Color16 c) {
	disconnect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
	m_table->insertRow(i);
	m_table->setItem(i, 0, mkCell(red16(c)));
	m_table->setItem(i, 1, mkCell(green16(c)));
	m_table->setItem(i, 2, mkCell(blue16(c)));
	m_table->setItem(i, 3, mkCell(alpha16(c)));
	m_table->setItem(i, 4, mkCell(toQColor(m_pal->colors[i]).name(QColor::HexArgb), false));
	m_table->item(i, 4)->setBackground(QColor(red32(c), green32(c), blue32(c), alpha32(c)));
	connect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
}

void PaletteEditor::rmTableRow(int idx) {
	disconnect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
	m_table->removeRow(idx);
	connect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
}

void PaletteEditor::setTableRow(int idx, Color16 c) {
	disconnect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
	m_table->item(idx, 0)->setText(QString::number(red16(c)));
	m_table->item(idx, 1)->setText(QString::number(green16(c)));
	m_table->item(idx, 2)->setText(QString::number(blue16(c)));
	m_table->item(idx, 3)->setText(QString::number(alpha16(c)));
	m_table->item(idx, 4)->setText(toQColor(m_pal->colors[idx]).name(QColor::HexArgb));
	connect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
}

void PaletteEditor::updateColor(int idx, Color16 c) {
	m_pal->colors[idx] = c;
	setTableRow(idx, c);
	setUnsavedChanges(true);
}

void PaletteEditor::saveItem() {
	m_ctx->project->writeObj(m_itemPath, m_pal.get());
}

void PaletteEditor::load() {
	disconnect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
	for (std::size_t i = 0; i < m_pal->colors.size(); ++i) {
		auto c = m_pal->colors[i];
		addTableRow(i, c);
	}
	connect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
}

Color16 PaletteEditor::rowColor(int row) const {
	auto r = static_cast<uint8_t>(m_table->item(row, 0)->text().toInt());
	auto g = static_cast<uint8_t>(m_table->item(row, 1)->text().toInt());
	auto b = static_cast<uint8_t>(m_table->item(row, 2)->text().toInt());
	auto a = static_cast<uint8_t>(m_table->item(row, 3)->text().toInt());
	return color16(r, g, b, a);
}

void PaletteEditor::colorSelected() {
	auto row = m_table->currentRow();
	if (row > -1) {
		m_rmBtn->setEnabled(true);
	} else {
		m_rmBtn->setEnabled(false);
	}
}

void PaletteEditor::cellChanged(int row, int) {
	auto oldColor = m_pal->colors[row];
	auto newColor = rowColor(row);
	undoStack()->push(new UpdateColorCommand(this, row, oldColor, newColor));
}

void PaletteEditor::rmColorClicked() {
	auto row = m_table->currentRow();
	undoStack()->push(new RemoveColorCommand(this, m_pal->colors[row], row));
}

}
