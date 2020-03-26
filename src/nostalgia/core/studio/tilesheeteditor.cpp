/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <array>

#include <QBuffer>
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

#include <nostalgia/core/consts.hpp>
#include <nostalgia/common/point.hpp>

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


class InsertTileCommand: public QUndoCommand {
	private:
		SheetData *m_sheetData = nullptr;
		int m_idx = 0;
		bool m_delete = false;
		QVector<int> m_tileRestore;

	public:
		InsertTileCommand(SheetData *sheetData, int idx, bool del = false) {
			m_sheetData = sheetData;
			m_idx = idx;
			m_delete = del;
		}

		virtual ~InsertTileCommand() = default;

		int id() const override {
			return static_cast<int>(CommandId::InsertTile);
		}

		void redo() override {
			if (m_delete) {
				m_tileRestore = m_sheetData->deleteTile(m_idx);
			} else {
				m_sheetData->insertTile(m_idx, m_tileRestore);
			}
		}

		void undo() override {
			if (m_delete) {
				m_sheetData->insertTile(m_idx, m_tileRestore);
			} else {
				m_tileRestore = m_sheetData->deleteTile(m_idx);
			}
		}

};


SheetData::SheetData(QUndoStack *undoStack): m_cmdStack(undoStack) {
}

void SheetData::updatePixel(QVariant pixelItem) {
	auto p = qobject_cast<QQuickItem*>(pixelItem.value<QObject*>());
	if (p && p != m_prevPixelUpdated) {
		m_cmdStack->push(new UpdatePixelsCommand(m_pixels, m_palette, p, m_selectedColor, m_cmdIdx));
		m_prevPixelUpdated = p;
		emit changeOccurred();
	}
}

void SheetData::beginCmd() {
	++m_cmdIdx;
}

void SheetData::endCmd() {
	m_prevPixelUpdated = nullptr;
}

void SheetData::insertTileCmd(int tileIdx) {
	m_cmdStack->push(new InsertTileCommand(this, tileIdx));
}

void SheetData::deleteTileCmd(int tileIdx) {
	m_cmdStack->push(new InsertTileCommand(this, tileIdx, true));
}

int SheetData::columns() const {
	return m_columns;
}

int SheetData::rows() const {
	return m_rows;
}

const QVector<int> &SheetData::pixels() const {
	return m_pixels;
}

QStringList SheetData::palette() const {
	return m_palette;
}

QString SheetData::palettePath() const {
	return m_currentPalettePath;
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

void SheetData::save(const studio::Context *ctx, QString ngPath) const {
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
	emit paletteChanged();
}

void SheetData::setPalette(const studio::Context *ctx, QString palPath) {
	std::unique_ptr<NostalgiaPalette> npal;
	try {
		npal = ctx->project->loadObj<NostalgiaPalette>(palPath);
	} catch (ox::Error) {
		qWarning() << "Could not open palette" << palPath;
	}
	if (npal) {
		setPalette(npal.get());
	}
	m_currentPalettePath = palPath;
	emit changeOccurred();
}

void SheetData::insertTile(int tileIdx, QVector<int> tileData) {
	auto pxIdx = tileIdx * PixelsPerTile;
	m_pixels.insert(pxIdx, PixelsPerTile, 0);
	std::copy(tileData.begin(), tileData.end(), &m_pixels[pxIdx]);
	emit pixelsChanged();
	emit changeOccurred();
}

QVector<int> SheetData::deleteTile(int tileIdx) {
	QVector<int> out;
	auto pxIdx = tileIdx * PixelsPerTile;
	std::copy(m_pixels.begin() + pxIdx,
	          m_pixels.begin() + (pxIdx + PixelsPerTile),
	          std::back_inserter(out));
	m_pixels.remove(pxIdx, PixelsPerTile);
	emit pixelsChanged();
	emit changeOccurred();
	return out;
}

void SheetData::setSelectedColor(int index) {
	m_selectedColor = index;
}

std::unique_ptr<NostalgiaGraphic> SheetData::toNostalgiaGraphic() const {
	auto ng = std::make_unique<NostalgiaGraphic>();
	const auto highestColorIdx = static_cast<uint8_t>(*std::max_element(m_pixels.begin(), m_pixels.end()));
	ng->defaultPalette = m_currentPalettePath.toUtf8().data();
	ng->bpp = highestColorIdx > 15 ? 8 : 4;
	ng->columns = m_columns;
	ng->rows = m_rows;
	auto pixelCount = ng->rows * ng->columns * PixelsPerTile;
	if (ng->bpp == 4) {
		ng->tiles.resize(pixelCount / 2);
		for (int i = 0; i < m_pixels.size(); ++i) {
			if (i & 1) {
				ng->tiles[i / 2] |= static_cast<uint8_t>(m_pixels[i]) << 4;
			} else {
				ng->tiles[i / 2] = static_cast<uint8_t>(m_pixels[i]);
			}
		}
	} else {
		ng->tiles.resize(pixelCount);
		for (std::size_t i = 0; i < ng->tiles.size(); ++i) {
			ng->tiles[i] = static_cast<uint8_t>(m_pixels[i]);
		}
	}
	return ng;
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
	m_cmdStack->push(new UpdateDimensionsCommand(this, UpdateDimensionsCommand::Dimension::Columns, m_columns, columns));
}

void SheetData::updateRows(int rows) {
	m_cmdStack->push(new UpdateDimensionsCommand(this, UpdateDimensionsCommand::Dimension::Rows, m_rows, rows));
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
}

TileSheetEditor::TileSheetEditor(QString path, const studio::Context *ctx, QWidget *parent): studio::Editor(parent), m_sheetData(undoStack()) {
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
	setPalette();
	setColorTable();
	restoreState();
	connect(&m_sheetData, &SheetData::changeOccurred, [this] { setUnsavedChanges(true); });
	connect(&m_sheetData, &SheetData::paletteChanged, this, &TileSheetEditor::setColorTable);
	setExportable(true);
}

TileSheetEditor::~TileSheetEditor() {
	saveState();
}

QString TileSheetEditor::itemName() {
	return m_itemName;
}

void TileSheetEditor::exportFile() {
	auto path = QFileDialog::getSaveFileName(this, tr("Export to Image"), "",
	                                         tr("PNG (*.png)"));
	auto ng = m_sheetData.toNostalgiaGraphic();
	QString palPath;
	if (palPath == "" && ng->defaultPalette.type() == ox::FileAddressType::Path) {
		palPath = ng->defaultPalette.getPath().value;
	}
	std::unique_ptr<NostalgiaPalette> npal;
	try {
		npal = m_ctx->project->loadObj<NostalgiaPalette>(palPath);
	} catch (ox::Error) {
		qWarning() << "Could not open palette" << palPath;
		// TODO: message box to notify of failure
	}
	toQImage(ng.get(), npal.get()).save(path, "PNG");
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
	connect(m_colorPicker.palette, &QComboBox::currentTextChanged, this, [this](QString name) {
		m_sheetData.setPalette(m_ctx, palettePath(name));
	});
	lyt->addWidget(m_colorPicker.palette);
	lyt->addWidget(m_colorPicker.colorTable);
	m_ctx->project->subscribe(studio::ProjectEvent::FileRecognized, m_colorPicker.palette, [this](QString path) {
		if (path.startsWith(PaletteDir) && path.endsWith(FileExt_npal)) {
			auto name = paletteName(path);
			m_colorPicker.palette->addItem(name);
		}
	});
	m_ctx->project->subscribe(studio::ProjectEvent::FileDeleted, m_colorPicker.palette, [this](QString path) {
		if (path.startsWith(PaletteDir) && path.endsWith(FileExt_npal)) {
			auto name = paletteName(path);
			auto idx = m_colorPicker.palette->findText(name);
			m_colorPicker.palette->removeItem(idx);
		}
	});
	return colorPicker;
}

void TileSheetEditor::setPalette() {
	auto name = paletteName(m_sheetData.palettePath());
	m_colorPicker.palette->setCurrentText(name);
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

QString TileSheetEditor::paletteName(QString palettePath) const {
	const auto begin = ox_strlen(PaletteDir);
	const auto end = palettePath.size() - (ox_strlen(FileExt_npal) + ox_strlen(PaletteDir));
	return palettePath.mid(begin, end);
}

QString TileSheetEditor::palettePath(QString paletteName) const {
	return PaletteDir + paletteName + FileExt_npal;
}

constexpr common::Point idxToPt(int i, int c, int) noexcept {
	common::Point p;
	const auto t = i / PixelsPerTile; // tile number
	const auto iti = i % PixelsPerTile; // in tile index
	const auto tc = t % c; // tile column
	const auto tr = t / c; // tile row
	const auto itx = iti % TileWidth; // in tile x
	const auto ity = iti / TileHeight; // in tile y
	return {
		itx + tc * TileWidth,
		ity + tr * TileHeight,
	};
}

static_assert(idxToPt(4, 1, 1) == common::Point{4, 0});
static_assert(idxToPt(8, 1, 1) == common::Point{0, 1});
static_assert(idxToPt(8, 2, 2) == common::Point{0, 1});
static_assert(idxToPt(64, 2, 2) == common::Point{8, 0});
static_assert(idxToPt(128, 2, 2) == common::Point{0, 8});
static_assert(idxToPt(129, 2, 2) == common::Point{1, 8});
static_assert(idxToPt(192, 2, 2) == common::Point{8, 8});
static_assert(idxToPt(384, 8, 6) == common::Point{48, 0});

QImage TileSheetEditor::toQImage(NostalgiaGraphic *ng, NostalgiaPalette *npal) const {
	const auto w = ng->columns * TileWidth;
	const auto h = ng->rows * TileHeight;
	QImage dst(w, h, QImage::Format_RGB32);
	auto setPixel = [&dst, npal, ng](std::size_t i, uint8_t p) {
		const auto color = toColor32(npal->colors[p]) >> 8;
		const auto pt = idxToPt(i, ng->columns, ng->rows);
		dst.setPixel(pt.x, pt.y, color);
	};
	if (ng->bpp == 4) {
		for (std::size_t i = 0; i < ng->tiles.size(); ++i) {
			auto p1 = ng->tiles[i] & 0xF;
			auto p2 = ng->tiles[i] >> 4;
			setPixel(i * 2 + 0, p1);
			setPixel(i * 2 + 1, p2);
		}
	} else {
		for (std::size_t i = 0; i < ng->tiles.size(); i++) {
			const auto p = ng->tiles[i];
			setPixel(i, p);
		}
	}
	return dst;
}

void TileSheetEditor::colorSelected() {
	m_sheetData.setSelectedColor(m_colorPicker.colorTable->currentRow());
}

void TileSheetEditor::setColorTable() {
	auto hexColors = m_sheetData.palette();
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

}
