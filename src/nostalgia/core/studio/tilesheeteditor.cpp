/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QHeaderView>
#include <QPointer>
#include <QQmlContext>
#include <QQuickWidget>
#include <QSet>
#include <QSettings>
#include <QSplitter>
#include <QUndoCommand>
#include <QVBoxLayout>

#include "consts.hpp"
#include "tilesheeteditor.hpp"

namespace nostalgia::core {

QColor toQColor(Color16 nc) {
	const auto r = red32(nc);
	const auto g = green32(nc);
	const auto b = blue32(nc);
	const auto a = 255;
	return QColor(r, g, b, a);
}


class UpdatePixelsCommand: public QUndoCommand {
	private:
		struct PixelUpdate {
			QPointer<QQuickItem> item;
			int oldColorId = 0;

			bool operator==(const PixelUpdate &o) const {
				return item == o.item;
			}

			bool operator<(const PixelUpdate &o) const {
				return item < o.item;
			}

			operator auto() const {
				return reinterpret_cast<quint64>(item.data());
			}
		};

		uint64_t m_cmdIdx = 0;
		int m_newColorId = 0;
		const QStringList &m_palette;
		QVector<int> &m_pixels;
		QSet<PixelUpdate> m_pixelUpdates;

	public:
		UpdatePixelsCommand(QVector<int> &pixels, const QStringList &palette, QQuickItem *pixelItem, int newColorId, uint64_t cmdIdx): m_palette(palette), m_pixels(pixels) {
			m_newColorId = newColorId;
			m_cmdIdx = cmdIdx;
			PixelUpdate pu;
			pu.item = pixelItem;
			pu.oldColorId = m_palette.indexOf(pixelItem->property("color").value<QColor>().name(QColor::HexArgb));
			m_pixelUpdates.insert(pu);
		}

		virtual ~UpdatePixelsCommand() = default;

		int id() const override {
			return static_cast<int>(CommandId::UpdatePixel);
		}

		bool mergeWith(const QUndoCommand *cmd) override {
			auto other = static_cast<const UpdatePixelsCommand*>(cmd);
			if (m_cmdIdx == other->m_cmdIdx) {
				m_pixelUpdates.unite(other->m_pixelUpdates);
				return true;
			}
			return false;
		}

		void redo() override {
			for (const auto &pu : m_pixelUpdates) {
				pu.item->setProperty("color", m_palette[m_newColorId]);
				m_pixels[pu.item->property("pixelNumber").toInt()] = m_newColorId;
			}
		}

		void undo() override {
			for (const auto &pu : m_pixelUpdates) {
				pu.item->setProperty("color", m_palette[pu.oldColorId]);
				m_pixels[pu.item->property("pixelNumber").toInt()] = pu.oldColorId;
			}
		}
};


void SheetData::updatePixel(QVariant pixelItem) {
	auto p = qobject_cast<QQuickItem*>(pixelItem.value<QObject*>());
	if (p && p != m_prevPixelUpdated) {
		m_cmdStack.push(new UpdatePixelsCommand(m_pixels, m_palette, p, m_selectedColor, m_cmdIdx));
		m_prevPixelUpdated = p;
	}
}

int SheetData::columns() {
	return m_columns;
}

void SheetData::setColumns(int columns) {
	m_columns = columns;
	emit columnsChanged();
}

int SheetData::rows() {
	return m_rows;
}

void SheetData::setRows(int rows) {
	m_rows = rows;
	emit rowsChanged();
}

const QVector<int> &SheetData::pixels() {
	return m_pixels;
}

QStringList SheetData::palette() {
	return m_palette;
}

void SheetData::updatePixels(const studio::Context *ctx, QString ngPath, QString palPath) {
	auto ng = ctx->project->loadObj<NostalgiaGraphic>(ngPath);
	std::unique_ptr<NostalgiaPalette> npal;
	if (palPath == "" && ng->defaultPalette.type() == ox::FileAddressType::Path) {
		palPath = ng->defaultPalette.getPath().value;
	}
	try {
		npal = ctx->project->loadObj<NostalgiaPalette>(palPath);
		qInfo() << "Opened palette" << palPath;
	} catch (ox::Error) {
		qWarning() << "Could not open palette" << palPath;
	}
	updatePixels(ng.get(), npal.get());
}

void SheetData::setSelectedColor(int index) {
	m_selectedColor = index;
}

QUndoStack *SheetData::undoStack() {
	return &m_cmdStack;
}

void SheetData::updatePixels(const NostalgiaGraphic *ng, const NostalgiaPalette *npal) {
	if (!npal) {
		npal = &ng->pal;
	}

	// load palette
	for (std::size_t i = 0; i < npal->colors.size(); i++) {
		const auto c = toQColor(npal->colors[i]);
		const auto color = c.name(QColor::HexArgb);
		m_palette.append(color);
	}

	if (ng->bpp == 8) {
		for (std::size_t i = 0; i < ng->tiles.size(); i++) {
			m_pixels.push_back(ng->tiles[i]);
		}
	} else {
		for (std::size_t i = 0; i < ng->tiles.size() * 2; i++) {
			uint8_t p;
			if (i & 1) {
				p = ng->tiles[i / 2] >> 4;
			} else {
				p = ng->tiles[i / 2] & 0xF;
			}
			m_pixels.push_back(p);
		}
	}
	emit pixelsChanged();
	emit paletteChanged();
}

void SheetData::beginCmd() {
	++m_cmdIdx;
}

void SheetData::endCmd() {
	m_prevPixelUpdated = nullptr;
}


TileSheetEditor::TileSheetEditor(QString path, const studio::Context *ctx, QWidget *parent): studio::Editor(parent) {
	m_ctx = ctx;
	m_itemName = path.mid(path.lastIndexOf('/'));
	auto lyt = new QVBoxLayout(this);
	m_splitter = new QSplitter(this);
	auto canvas = new QQuickWidget(m_splitter);
	lyt->addWidget(m_splitter);
	m_splitter->addWidget(canvas);
	m_splitter->addWidget(setupColorPicker(m_splitter));
	m_splitter->setStretchFactor(0, 1);
	m_sheetData.updatePixels(m_ctx, path);
	canvas->rootContext()->setContextProperty("sheetData", &m_sheetData);
	canvas->setSource(QUrl::fromLocalFile(":/qml/TileSheetEditor.qml"));
	canvas->setResizeMode(QQuickWidget::SizeRootObjectToView);
	setColorTable(m_sheetData.palette());
	restoreState();
}

TileSheetEditor::~TileSheetEditor() {
	saveState();
}

QString TileSheetEditor::itemName() {
	return m_itemName;
}

void TileSheetEditor::save() {
}

QUndoStack *TileSheetEditor::undoStack() {
	return m_sheetData.undoStack();
}

QWidget *TileSheetEditor::setupColorPicker(QWidget *parent) {
	auto colorPicker = new QWidget(parent);
	auto lyt = new QVBoxLayout(colorPicker);
	m_colorPicker.palette = new QComboBox(colorPicker);
	m_colorPicker.colorTable = new QTableWidget(colorPicker);
	m_colorPicker.colorTable->setColumnCount(2);
	m_colorPicker.colorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_colorPicker.colorTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_colorPicker.colorTable->setHorizontalHeaderLabels(QStringList() << tr("Hex Code") << tr("Color"));
	m_colorPicker.colorTable->horizontalHeader()->setStretchLastSection(true);
	m_colorPicker.colorTable->verticalHeader()->hide();
	connect(m_colorPicker.colorTable, &QTableWidget::itemSelectionChanged, this, &TileSheetEditor::colorSelected);
	lyt->addWidget(m_colorPicker.palette);
	lyt->addWidget(m_colorPicker.colorTable);
	return colorPicker;
}

void TileSheetEditor::setColorTable(QStringList hexColors) {
	auto tbl = m_colorPicker.colorTable;
	tbl->setRowCount(hexColors.size());
	for (int i = 0; i < hexColors.size(); i++) {
		auto hexCode = new QTableWidgetItem;
		hexCode->setText(hexColors[i]);
		hexCode->setFont(QFont("monospace"));
		tbl->setItem(i, 0, hexCode);
		auto color = new QTableWidgetItem;
		color->setBackground(QColor(hexColors[i]));
		tbl->setItem(i, 1, color);
	}
}

void TileSheetEditor::saveState() {
	QSettings settings(m_ctx->orgName, PluginName);
	settings.beginGroup("TileSheetEditor");
	settings.setValue("m_splitter/state", m_splitter->saveState());
	settings.endGroup();
}

void TileSheetEditor::restoreState() {
	QSettings settings(m_ctx->orgName, PluginName);
	settings.beginGroup("TileSheetEditor");
	m_splitter->restoreState(settings.value("m_splitter/state", m_splitter->saveState()).toByteArray());
	settings.endGroup();
}

void TileSheetEditor::colorSelected() {
	m_sheetData.setSelectedColor(m_colorPicker.colorTable->currentRow());
}

}
