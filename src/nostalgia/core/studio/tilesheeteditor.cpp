/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QHBoxLayout>
#include <QHeaderView>
#include <QPointer>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QSet>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QUndoCommand>
#include <QTableWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <memory>

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


struct LabeledSpinner: public QWidget {

	QSpinBox *const spinBox = new QSpinBox(this);

	LabeledSpinner(QString name, int minimum, int value) {
		auto lyt = new QHBoxLayout;
		setLayout(lyt);
		auto lbl = new QLabel(name, this);
		lbl->setBuddy(spinBox);
		lyt->addWidget(lbl);
		lyt->addWidget(spinBox);
		spinBox->setMinimum(minimum);
		spinBox->setValue(value);
	}

	virtual ~LabeledSpinner() = default;

};


class UpdateDimensionsCommand: public QUndoCommand {
	public:
		enum class Dimension {
			Rows,
			Columns,
		};

	private:
		Dimension m_dimension = Dimension::Rows;
		int m_oldVal = 0;
		int m_newVal = 0;
		SheetData *m_sheetData = nullptr;

	public:
		UpdateDimensionsCommand(SheetData *sheetData, Dimension dim, int oldVal, int newVal) {
			m_sheetData = sheetData;
			m_dimension = dim;
			m_newVal = newVal;
			m_oldVal = oldVal;
			setObsolete(newVal == oldVal);
		}

		virtual ~UpdateDimensionsCommand() = default;

		int id() const override {
			return static_cast<int>(CommandId::UpdateDimension);
		}

		void redo() override {
			switch (m_dimension) {
				case Dimension::Rows:
					m_sheetData->setRows(m_newVal);
					break;
				case Dimension::Columns:
					m_sheetData->setColumns(m_newVal);
					break;
			}
		}

		void undo() override {
			switch (m_dimension) {
				case Dimension::Rows:
					m_sheetData->setRows(m_oldVal);
					break;
				case Dimension::Columns:
					m_sheetData->setColumns(m_oldVal);
					break;
			}
		}

};


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

			operator quint64() const {
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
			setObsolete(pu.oldColorId == newColorId);
		}

		virtual ~UpdatePixelsCommand() = default;

		int id() const override {
			return static_cast<int>(CommandId::UpdatePixel);
		}

		bool mergeWith(const QUndoCommand *cmd) override {
			auto other = dynamic_cast<const UpdatePixelsCommand*>(cmd);
			if (other && m_cmdIdx == other->m_cmdIdx) {
				m_pixelUpdates.unite(other->m_pixelUpdates);
				return true;
			}
			return false;
		}

		void redo() override {
			for (const auto &pu : m_pixelUpdates) {
				pu.item->setProperty("color", m_palette[m_newColorId]);
				const auto index = pu.item->property("pixelNumber").toInt();
				// resize to appropriate number of tiles if pixel beyond current
				// range
				if (m_pixels.size() < index) {
					auto sz = (index / 64 + 1) * 64;
					m_pixels.resize(sz);
				}
				m_pixels[index] = m_newColorId;
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
		emit changeOccurred();
	}
}

int SheetData::columns() {
	return m_columns;
}

int SheetData::rows() {
	return m_rows;
}

const QVector<int> &SheetData::pixels() {
	return m_pixels;
}

QStringList SheetData::palette() {
	return m_palette;
}

void SheetData::load(const studio::Context *ctx, QString ngPath, QString palPath) {
	auto ng = ctx->project->loadObj<NostalgiaGraphic>(ngPath);
	if (palPath == "" && ng->defaultPalette.type() == ox::FileAddressType::Path) {
		palPath = ng->defaultPalette.getPath().value;
	}
	m_tilesheetPath = ngPath;
	m_currentPalettePath = palPath;
	setRows(ng->rows);
	setColumns(ng->columns);
	if (palPath != "") {
		setPalette(ctx, palPath);
	} else {
		setPalette(&ng->pal);
	}
	updatePixels(ng.get());
}

void SheetData::save(const studio::Context *ctx, QString ngPath) {
	auto ng = toNostalgiaGraphic();
	ctx->project->writeObj(ngPath, ng.get());
}

void SheetData::setPalette(const NostalgiaPalette *npal) {
	// load palette
	m_palette.clear();
	for (std::size_t i = 0; i < npal->colors.size(); i++) {
		const auto c = toQColor(npal->colors[i]);
		const auto color = c.name(QColor::HexArgb);
		m_palette.append(color);
	}
}

void SheetData::setPalette(const studio::Context *ctx, QString palPath) {
	std::unique_ptr<NostalgiaPalette> npal;
	try {
		npal = ctx->project->loadObj<NostalgiaPalette>(palPath);
		qInfo() << "Opened palette" << palPath;
	} catch (ox::Error) {
		qWarning() << "Could not open palette" << palPath;
	}
	if (npal) {
		setPalette(npal.get());
	}
}

void SheetData::setSelectedColor(int index) {
	m_selectedColor = index;
}

QUndoStack *SheetData::undoStack() {
	return &m_cmdStack;
}

void SheetData::setColumns(int columns) {
	m_columns = columns;
	emit columnsChanged(columns);
	emit changeOccurred();
}

void SheetData::setRows(int rows) {
	m_rows = rows;
	emit rowsChanged(rows);
	emit changeOccurred();
}

void SheetData::updateColumns(int columns) {
	m_cmdStack.push(new UpdateDimensionsCommand(this, UpdateDimensionsCommand::Dimension::Columns, m_columns, columns));
}

void SheetData::updateRows(int rows) {
	m_cmdStack.push(new UpdateDimensionsCommand(this, UpdateDimensionsCommand::Dimension::Rows, m_rows, rows));
}

void SheetData::updatePixels(const NostalgiaGraphic *ng) {
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

std::unique_ptr<NostalgiaGraphic> SheetData::toNostalgiaGraphic() {
	auto ng = std::make_unique<NostalgiaGraphic>();
	const auto highestColorIdx = static_cast<uint8_t>(*std::max_element(m_pixels.begin(), m_pixels.end()));
	ng->defaultPalette = m_currentPalettePath.toUtf8().data();
	ng->bpp = highestColorIdx > 15 ? 8 : 4;
	ng->columns = m_columns;
	ng->rows = m_rows;
	if (ng->bpp == 4) {
		ng->tiles.resize(m_pixels.size() / 2);
		for (int i = 0; i < m_pixels.size(); ++i) {
			if (i & 1) {
				ng->tiles[i / 2] |= static_cast<uint8_t>(m_pixels[i]) << 4;
			} else {
				ng->tiles[i / 2] = static_cast<uint8_t>(m_pixels[i]);
			}
		}
	} else {
		ng->tiles.resize(m_pixels.size());
		for (int i = 0; i < m_pixels.size(); ++i) {
			ng->tiles[i] = static_cast<uint8_t>(m_pixels[i]);
		}
	}
	return ng;
}

void SheetData::beginCmd() {
	++m_cmdIdx;
}

void SheetData::endCmd() {
	m_prevPixelUpdated = nullptr;
}


TileSheetEditor::TileSheetEditor(QString path, const studio::Context *ctx, QWidget *parent): studio::Editor(parent) {
	m_ctx = ctx;
	m_itemPath = path;
	m_itemName = path.mid(path.lastIndexOf('/'));
	auto lyt = new QVBoxLayout(this);
	m_splitter = new QSplitter(this);
	auto canvasParent = new QWidget(m_splitter);
	auto canvasLyt = new QVBoxLayout(canvasParent);
	m_canvas = new QQuickWidget(canvasParent);
	canvasLyt->addWidget(m_canvas);
	auto tb = new QToolBar(tr("Tile Sheet Options"));
	m_tilesX = new LabeledSpinner(tr("Tiles &X:"), 1, m_sheetData.columns());
	m_tilesY = new LabeledSpinner(tr("Tiles &Y:"), 1, m_sheetData.rows());
	tb->addWidget(m_tilesX);
	tb->addWidget(m_tilesY);
	canvasLyt->setMenuBar(tb);
	lyt->addWidget(m_splitter);
	m_splitter->addWidget(canvasParent);
	m_splitter->addWidget(setupColorPicker(m_splitter));
	m_splitter->setStretchFactor(0, 1);
	connect(&m_sheetData, &SheetData::columnsChanged, [this](int val) {
		disconnect(m_tilesX->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), &m_sheetData, &SheetData::updateColumns);
		m_tilesX->spinBox->setValue(val);
		connect(m_tilesX->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), &m_sheetData, &SheetData::updateColumns);
	});
	connect(&m_sheetData, &SheetData::rowsChanged, [this](int val) {
		disconnect(m_tilesY->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), &m_sheetData, &SheetData::updateRows);
		m_tilesY->spinBox->setValue(val);
		connect(m_tilesY->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), &m_sheetData, &SheetData::updateRows);
	});
	m_sheetData.load(m_ctx, m_itemPath);
	connect(m_tilesX->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), &m_sheetData, &SheetData::updateColumns);
	connect(m_tilesY->spinBox, QOverload<int>::of(&QSpinBox::valueChanged), &m_sheetData, &SheetData::updateRows);
	m_canvas->rootContext()->setContextProperty("sheetData", &m_sheetData);
	m_canvas->setSource(QUrl::fromLocalFile(":/qml/TileSheetEditor.qml"));
	m_canvas->setResizeMode(QQuickWidget::SizeRootObjectToView);
	setColorTable(m_sheetData.palette());
	restoreState();
	connect(&m_sheetData, &SheetData::changeOccurred, [this] { setUnsavedChanges(true); });
}

TileSheetEditor::~TileSheetEditor() {
	saveState();
}

QString TileSheetEditor::itemName() {
	return m_itemName;
}

QUndoStack *TileSheetEditor::undoStack() {
	return m_sheetData.undoStack();
}

void TileSheetEditor::saveItem() {
	m_sheetData.save(m_ctx, m_itemPath);
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
	settings.beginGroup("TileSheetEditor/" + m_itemName);
	settings.setValue("m_splitter/state", m_splitter->saveState());
	settings.setValue("m_sheetData/tileRows", m_sheetData.rows());
	settings.setValue("m_sheetData/tileColumns", m_sheetData.columns());
	settings.setValue("m_colorPicker.colorTable/geometry", m_colorPicker.colorTable->horizontalHeader()->saveState());
	settings.endGroup();
}

void TileSheetEditor::restoreState() {
	QSettings settings(m_ctx->orgName, PluginName);
	settings.beginGroup("TileSheetEditor/" + m_itemName);
	m_splitter->restoreState(settings.value("m_splitter/state", m_splitter->saveState()).toByteArray());
	m_colorPicker.colorTable->horizontalHeader()->restoreState(settings.value("m_colorPicker.colorTable/geometry", m_colorPicker.colorTable->horizontalHeader()->saveState()).toByteArray());
	settings.endGroup();
}

void TileSheetEditor::colorSelected() {
	m_sheetData.setSelectedColor(m_colorPicker.colorTable->currentRow());
}

}
