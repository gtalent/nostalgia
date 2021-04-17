/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QHeaderView>
#include <QItemDelegate>
#include <QPainter>
#include <QPushButton>
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
	MoveColor,
};


class ColorChannelValidator: public QValidator {

	public:
		explicit ColorChannelValidator(QLineEdit *parent);

		QValidator::State validate(QString &input, int&) const override;

	private:
		[[nodiscard]] static QString convert(const QString &input);

};

ColorChannelValidator::ColorChannelValidator(QLineEdit *parent): QValidator(parent) {
	connect(parent, &QLineEdit::editingFinished, [parent] {
		parent->setText(convert(parent->text()));
	});
}

QString ColorChannelValidator::convert(const QString &input) {
	int num = 0;
	if (input[0] == '_') {
		num = input.mid(1).toInt() >> 3;
	}
	return QString::number(num);
}

QValidator::State ColorChannelValidator::validate(QString &input, int&) const {
	if (input.size() == 0) {
		return QValidator::State::Intermediate;
	}
	const auto convert = input[0] == '_';
	const auto max = convert ? 255 : 31;
	const auto numTxt = convert ? input.mid(1) : input;
	bool isNumber = false;
	const auto num = numTxt.toInt(&isNumber);
	if (isNumber && num >= 0 && num <= max) {
		return QValidator::State::Acceptable;
	} else if (numTxt == "") {
		return QValidator::State::Intermediate;
	} else {
		return QValidator::State::Invalid;
	}
}


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

		~AddColorCommand() override = default;

		[[nodiscard]] int id() const override {
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

		~RemoveColorCommand() override = default;

		[[nodiscard]] int id() const override {
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

		~UpdateColorCommand() override = default;

		[[nodiscard]] int id() const override {
			return static_cast<int>(PaletteEditorCommandId::UpdateColor);
		}

		void redo() override {
			m_editor->updateColor(m_idx, m_newColor);
		}

		void undo() override {
			m_editor->updateColor(m_idx, m_oldColor);
		}

};

class MoveColorCommand: public QUndoCommand {
	private:
		PaletteEditor *m_editor = nullptr;
		int m_idx = -1;
		int m_offset = 0;

	public:
		MoveColorCommand(PaletteEditor *editor, int idx, int offset) {
			m_editor = editor;
			m_idx = idx;
			m_offset = offset;
		}

		~MoveColorCommand() override = default;

		[[nodiscard]] int id() const override {
			return static_cast<int>(PaletteEditorCommandId::MoveColor);
		}

		void redo() override {
			m_editor->moveColor(m_idx, m_offset);
		}

		void undo() override {
			m_editor->moveColor(m_idx + m_offset, -m_offset);
		}

};


QWidget *PaletteEditorColorTableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex &idx) const {
	auto le = new QLineEdit(parent);
	if (idx.column()) {
		auto validator = new ColorChannelValidator(le);
		le->setValidator(validator);
	}
	return le;
}

void PaletteEditorColorTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &idx) const {
	if (idx.column() != 3) {
		QStyledItemDelegate::paint(painter, opt, idx);
	} else {
		auto color = idx.model()->data(idx, Qt::DisplayRole).toString();
		painter->fillRect(opt.rect, QColor(color));
	}
}


static QTableWidgetItem *mkCell(const QString& v, bool editable = true) {
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
	m_addBtn = new QPushButton(tr("Add"), tb);
	m_rmBtn = new QPushButton(tr("Remove"), tb);
	m_moveUpBtn = new QPushButton(tr("Move Up"), tb);
	m_moveDownBtn = new QPushButton(tr("Move Down"), tb);
	m_rmBtn->setEnabled(false);
	m_moveUpBtn->setEnabled(false);
	m_moveDownBtn->setEnabled(false);
	tb->addWidget(m_addBtn);
	tb->addWidget(m_rmBtn);
	tb->addWidget(m_moveUpBtn);
	tb->addWidget(m_moveDownBtn);
	canvasLyt->setMenuBar(tb);

	m_table = new QTableWidget(this);
	m_table->setItemDelegate(&m_colorTableDelegate);
	m_table->setColumnCount(4);
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->setHorizontalHeaderLabels(QStringList() << tr("Red") << tr("Green") << tr("Blue") << tr("Color Preview"));
	m_table->horizontalHeader()->setStretchLastSection(true);
	m_table->verticalHeader()->hide();
	canvasLyt->addWidget(m_table);
	connect(m_table, &QTableWidget::itemSelectionChanged, this, &PaletteEditor::colorSelected);
	connect(m_addBtn, &QPushButton::clicked, this, &PaletteEditor::addColorClicked);
	connect(m_rmBtn, &QPushButton::clicked, this, &PaletteEditor::rmColorClicked);
	connect(m_moveUpBtn, &QPushButton::clicked, this, &PaletteEditor::moveColorUpClicked);
	connect(m_moveDownBtn, &QPushButton::clicked, this, &PaletteEditor::moveColorDownClicked);
	connect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
	m_pal = m_ctx->project->loadObj<NostalgiaPalette>(m_itemPath);
	load();
}

QString PaletteEditor::itemName() const {
	return m_itemPath.mid(m_itemPath.lastIndexOf('/'));
}

void PaletteEditor::addTableRow(int i, Color16 c) {
	disconnect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
	m_table->insertRow(i);
	m_table->setItem(i, 0, mkCell(red16(c)));
	m_table->setItem(i, 1, mkCell(green16(c)));
	m_table->setItem(i, 2, mkCell(blue16(c)));
	m_table->setItem(i, 3, mkCell(toQColor(m_pal->colors[static_cast<std::size_t>(i)]).name(QColor::HexRgb), false));
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
	m_table->item(idx, 3)->setText(toQColor(m_pal->colors[static_cast<std::size_t>(idx)]).name(QColor::HexRgb));
	connect(m_table, &QTableWidget::cellChanged, this, &PaletteEditor::cellChanged);
}

void PaletteEditor::addColor(int idx, Color16 c) {
	m_pal->colors.insert(static_cast<std::size_t>(idx), c);
	addTableRow(idx, c);
	setUnsavedChanges(true);
}

void PaletteEditor::rmColor(int idx) {
	rmTableRow(idx);
	oxIgnoreError(m_pal->colors.erase(static_cast<std::size_t>(idx)));
	setUnsavedChanges(true);
}

void PaletteEditor::updateColor(int idx, Color16 c) {
	m_pal->colors[static_cast<std::size_t>(idx)] = c;
	setTableRow(idx, c);
	setUnsavedChanges(true);
}

void PaletteEditor::moveColor(int idx, int offset) {
	auto c = m_pal->colors[static_cast<std::size_t>(idx)];
	oxIgnoreError(m_pal->colors.erase(static_cast<std::size_t>(idx)));
	m_pal->colors.insert(static_cast<std::size_t>(idx + offset), c);
	rmTableRow(idx);
	addTableRow(idx + offset, c);
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
	return color16(r, g, b);
}

void PaletteEditor::colorSelected() {
	auto selIdxs = m_table->selectionModel()->selectedIndexes();
	auto row = !selIdxs.empty() ? selIdxs[0].row() : -1;
	if (row > -1) {
		m_rmBtn->setEnabled(true);
		m_moveUpBtn->setEnabled(row > 0);
		m_moveDownBtn->setEnabled(row < m_table->rowCount() - 1);
	} else {
		m_rmBtn->setEnabled(false);
		m_moveUpBtn->setEnabled(false);
		m_moveDownBtn->setEnabled(false);
	}
}

void PaletteEditor::cellChanged(int row, int) {
	auto oldColor = m_pal->colors[static_cast<std::size_t>(row)];
	auto newColor = rowColor(row);
	undoStack()->push(new UpdateColorCommand(this, row, oldColor, newColor));
}

void PaletteEditor::addColorClicked() {
	auto row = m_table->rowCount();
	undoStack()->push(new AddColorCommand(this, 0, row));
}

void PaletteEditor::rmColorClicked() {
	auto row = m_table->currentRow();
	undoStack()->push(new RemoveColorCommand(this, m_pal->colors[static_cast<std::size_t>(row)], row));
}

void PaletteEditor::moveColorUpClicked() {
	auto row = m_table->currentRow();
	undoStack()->push(new MoveColorCommand(this, row, -1));
}

void PaletteEditor::moveColorDownClicked() {
	auto row = m_table->currentRow();
	undoStack()->push(new MoveColorCommand(this, row, 1));
}

}
