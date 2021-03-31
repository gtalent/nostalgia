/*
 * Copyright 2016 - 2021 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <memory>

#include <QDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QSettings>
#include <QSpinBox>
#include <QSplitter>
#include <QTableWidget>
#include <QToolBar>
#include <QUndoCommand>

#include <nostalgia/core/consts.hpp>
#include <nostalgia/common/point.hpp>

#include "consts.hpp"
#include "imgconv.hpp"
#include "util.hpp"
#include "tilesheeteditor.hpp"

namespace nostalgia::core {

[[nodiscard]] constexpr int ptToIdx(int x, int y, int c) noexcept {
	return pointToIdx(c * TileWidth, x, y);
}

[[nodiscard]] constexpr int ptToIdx(common::Point pt, int c) noexcept {
	return pointToIdx(c * TileWidth, pt.x, pt.y);
}

[[nodiscard]] constexpr common::Point idxToPt(int i, int c) noexcept {
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

static_assert(idxToPt(4, 1) == common::Point{4, 0});
static_assert(idxToPt(8, 1) == common::Point{0, 1});
static_assert(idxToPt(8, 2) == common::Point{0, 1});
static_assert(idxToPt(64, 2) == common::Point{8, 0});
static_assert(idxToPt(128, 2) == common::Point{0, 8});
static_assert(idxToPt(129, 2) == common::Point{1, 8});
static_assert(idxToPt(192, 2) == common::Point{8, 8});
static_assert(idxToPt(384, 8) == common::Point{48, 0});

class ModAfterDialog: public QDialog {

	private:
		QComboBox *const m_afterColor = new QComboBox(this);
		QSpinBox *const m_mod = new QSpinBox(this);

	public:
		ModAfterDialog(const QStringList &palette, QWidget *parent = nullptr): QDialog(parent) {
			auto range = palette.size();
			auto okBtn = new QPushButton(tr("&OK"), this);
			setWindowTitle(tr("Mod Colors After"));
			setWindowModality(Qt::WindowModality::ApplicationModal);
			for (int i = 0; i < palette.size(); ++i) {
				const auto c = palette[i];
				m_afterColor->addItem(QString("%1: %2").arg(i + 1).arg(c));
			}
			m_mod->setMinimum(-range + 1);
			m_mod->setMaximum(range - 1);
			auto lyt = new QFormLayout(this);
			lyt->addRow(tr("&Greater/Equal To:"), m_afterColor);
			lyt->addRow(tr("&Mod By:"), m_mod);
			lyt->addWidget(okBtn);
			connect(okBtn, &QPushButton::clicked, this, &ModAfterDialog::accept);
		}

		int color() const {
			return m_afterColor->currentIndex() - 1;
		}

		int mod() const {
			return m_mod->value();
		}

};

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


class ModPixelsCommand: public QUndoCommand {
	private:
		SheetData *m_sheetData = nullptr;
		QHash<int, int> m_pixelUpdates;

	public:
		ModPixelsCommand(SheetData *sheetData): m_sheetData(sheetData) {
		}

		virtual ~ModPixelsCommand() = default;

		void addPixel(int index, int mod) {
			m_pixelUpdates[index] = mod;
		}

		int id() const override {
			return static_cast<int>(CommandId::ModPixel);
		}

		void redo() override {
			m_sheetData->modPixels(m_pixelUpdates, 1);
		}

		void undo() override {
			m_sheetData->modPixels(m_pixelUpdates, -1);
		}

};

class FillPixelsCommand: public QUndoCommand {
	private:
		const std::array<bool, PixelsPerTile> m_pixelUpdateMap;
		const std::size_t m_idx = 0;
		const int m_oldColor = 0;
		const int m_newColor = 0;
		SheetData *const m_sheetData = nullptr;

	public:
		FillPixelsCommand(SheetData *sheetData, const std::array<bool, PixelsPerTile> &pum, int idx, int oldColor, int newColor):
			m_pixelUpdateMap(pum),
			m_idx(static_cast<std::size_t>(idx)),
			m_oldColor(oldColor),
			m_newColor(newColor),
			m_sheetData(sheetData) {
		}

		virtual ~FillPixelsCommand() = default;

		int id() const override {
			return static_cast<int>(CommandId::ModPixel);
		}

		void redo() override {
			for (std::size_t i = 0; i < m_pixelUpdateMap.size(); ++i) {
				if (m_pixelUpdateMap[i]) {
					m_sheetData->setPixel(i + m_idx, m_newColor);
				}
			}
			m_sheetData->notifyUpdate();
		}

		void undo() override {
			for (std::size_t i = 0; i < m_pixelUpdateMap.size(); ++i) {
				if (m_pixelUpdateMap[i]) {
					m_sheetData->setPixel(i + m_idx, m_oldColor);
				}
			}
			m_sheetData->notifyUpdate();
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
		QVector<int> &m_pixelSelected;
		QSet<PixelUpdate> m_pixelUpdates;

	public:
		UpdatePixelsCommand(QVector<int> &pixels, QVector<int> &pixelSelected, const QStringList &palette, QQuickItem *pixelItem, int newColorId, uint64_t cmdIdx): m_palette(palette), m_pixels(pixels), m_pixelSelected(pixelSelected) {
			m_newColorId = newColorId;
			m_cmdIdx = cmdIdx;
			PixelUpdate pu;
			pu.item = pixelItem;
			pu.oldColorId = m_palette.indexOf(pixelItem->property("color").value<QColor>().name(QColor::HexRgb));
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
					m_pixelSelected.resize(sz);
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

class PasteClipboardCommand: public QUndoCommand {
	private:
		SheetData *m_sheetData = nullptr;
		std::unique_ptr<TileSheetClipboard> m_restore;
		std::unique_ptr<TileSheetClipboard> m_apply;

	public:
		PasteClipboardCommand(SheetData *sheetData,
		                      std::unique_ptr<TileSheetClipboard> &&restore,
		                      std::unique_ptr<TileSheetClipboard> &&apply): m_sheetData(sheetData), m_restore(std::move(restore)), m_apply(std::move(apply)) {
		}

		virtual ~PasteClipboardCommand() = default;

		int id() const override {
			return static_cast<int>(CommandId::ClipboardPaste);
		}

		void redo() override {
			m_sheetData->applyClipboard(*m_apply);
		}

		void undo() override {
			m_sheetData->applyClipboard(*m_restore);
		}

};


void TileSheetClipboard::addPixel(int color) {
	m_pixels.push_back(color);
}

void TileSheetClipboard::clear() {
	m_pixels.clear();
}

bool TileSheetClipboard::empty() const {
	return m_pixels.empty();
}

void TileSheetClipboard::pastePixels(common::Point pt, QVector<int> *tgtPixels, int tgtColumns) const {
	std::size_t srcIdx = 0;
	const auto w = m_p2.x - m_p1.x;
	const auto h = m_p2.y - m_p1.y;
	for (int x = 0; x <= w; ++x) {
		for (int y = 0; y <= h; ++y) {
			const auto tgtIdx = ptToIdx(pt + common::Point(x, y), tgtColumns);
			if (tgtIdx < tgtPixels->size()) {
				(*tgtPixels)[tgtIdx] = m_pixels[srcIdx];
			}
			++srcIdx;
		}
	}
}

void TileSheetClipboard::setPoints(common::Point p1, common::Point p2) {
	m_p1 = p1;
	m_p2 = p2;
}

common::Point TileSheetClipboard::point1() const {
	return m_p1;
}

common::Point TileSheetClipboard::point2() const {
	return m_p2;
}


void TileSheetEditorColorTableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &idx) const {
	if (idx.column() != 1) {
		QStyledItemDelegate::paint(painter, opt, idx);
	} else {
		auto color = idx.model()->data(idx, Qt::DisplayRole).toString();
		painter->fillRect(opt.rect, QColor(color));
	}
}


SheetData::SheetData(QUndoStack *undoStack): m_cmdStack(undoStack) {
}

void SheetData::fillPixel(QVariant pixelItem) {
	const auto p = qobject_cast<QQuickItem*>(pixelItem.value<QObject*>());
	if (p && p != m_prevPixelOperand) {
		const auto idx = p->property("pixelNumber").toInt();
		const auto pt = idxToPt(idx, m_columns);
		const auto oldColor = m_pixels[idx];
		std::array<bool, PixelsPerTile> pixels = {};
		getFillPixels(pixels.data(), pt, oldColor);
		m_cmdStack->push(new FillPixelsCommand(this, pixels, idx / PixelsPerTile * PixelsPerTile, oldColor, m_selectedColor));
	}
}

void SheetData::selectPixel(QVariant pixelItem) {
	auto p = qobject_cast<QQuickItem*>(pixelItem.value<QObject*>());
	if (p && p != m_prevPixelOperand) {
		const auto index = p->property("pixelNumber").toInt();
		const auto point = idxToPt(index, m_columns);
		m_prevPixelOperand = p;
		if (m_selectionStart == common::Point{-1, -1}) {
			m_selectionStart = point;
		}
		markSelection(point);
	}
}

void SheetData::updatePixel(QVariant pixelItem) {
	auto p = qobject_cast<QQuickItem*>(pixelItem.value<QObject*>());
	if (p && p != m_prevPixelOperand) {
		m_cmdStack->push(new UpdatePixelsCommand(m_pixels, m_pixelSelected, m_palette, p, m_selectedColor, m_cmdIdx));
		m_prevPixelOperand = p;
		emit changeOccurred();
	}
}

void SheetData::beginCmd() {
	++m_cmdIdx;
	m_selectionStart = {-1, -1};
	m_selectionEnd = {-1, -1};
	emit pixelSelectedChanged(0);
}

void SheetData::endCmd() {
	m_prevPixelOperand = nullptr;
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

const QVector<int> &SheetData::pixelSelected() const {
	return m_pixelSelected;
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

void SheetData::getFillPixels(bool *pixels, common::Point pt, int oldColor) const {
	const auto tileIdx = [this](const common::Point &pt) noexcept {
		return ptToIdx(pt, m_columns) / PixelsPerTile;
	};
	// get points
	const auto leftPt = pt + common::Point(-1, 0);
	const auto rightPt = pt + common::Point(1, 0);
	const auto topPt = pt + common::Point(0, -1);
	const auto bottomPt = pt + common::Point(0, 1);
	// calculate indices
	const auto idx = ptToIdx(pt, m_columns);
	const auto leftIdx = ptToIdx(leftPt, m_columns);
	const auto rightIdx = ptToIdx(rightPt, m_columns);
	const auto topIdx = ptToIdx(topPt, m_columns);
	const auto bottomIdx = ptToIdx(bottomPt, m_columns);
	const auto tile = tileIdx(pt);
	// mark pixels to update
	pixels[idx % PixelsPerTile] = true;
	if (!pixels[leftIdx % PixelsPerTile] && tile == tileIdx(leftPt) && m_pixels[leftIdx] == oldColor) {
		getFillPixels(pixels, leftPt, oldColor);
	}
	if (!pixels[rightIdx % PixelsPerTile] && tile == tileIdx(rightPt) && m_pixels[rightIdx] == oldColor) {
		getFillPixels(pixels, rightPt, oldColor);
	}
	if (!pixels[topIdx % PixelsPerTile] && tile == tileIdx(topPt) && m_pixels[topIdx] == oldColor) {
		getFillPixels(pixels, topPt, oldColor);
	}
	if (!pixels[bottomIdx % PixelsPerTile] && tile == tileIdx(bottomPt) && m_pixels[bottomIdx] == oldColor) {
		getFillPixels(pixels, bottomPt, oldColor);
	}
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

void SheetData::reload(const studio::Context *ctx) {
	load(ctx, m_tilesheetPath, m_currentPalettePath);
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
		const auto color = c.name(QColor::HexRgb);
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
	m_pixelSelected.insert(pxIdx, PixelsPerTile, 0);
	std::copy(tileData.begin(), tileData.end(), &m_pixels[pxIdx]);
	notifyUpdate();
}

QVector<int> SheetData::deleteTile(int tileIdx) {
	QVector<int> out;
	auto pxIdx = tileIdx * PixelsPerTile;
	std::copy(m_pixels.begin() + pxIdx,
	          m_pixels.begin() + (pxIdx + PixelsPerTile),
	          std::back_inserter(out));
	m_pixels.remove(pxIdx, PixelsPerTile);
	m_pixelSelected.remove(pxIdx, PixelsPerTile);
	notifyUpdate();
	return out;
}

void SheetData::setSelectedColor(int index) {
	m_selectedColor = index;
}

void SheetData::modGteCmd(int color, int mod) {
	auto cmd = new ModPixelsCommand(this);
	for (int i = 0; i < m_pixels.size(); ++i) {
		const auto p = m_pixels[i];
		if (p >= color) {
			const int mx = (m_palette.size() - p) - 1;
			cmd->addPixel(i, std::clamp(mod, -p, mx));
		}
	}
	m_cmdStack->push(cmd);
}

void SheetData::modPixels(const QHash<int, int> &mods, int inversion) {
	for (const auto index : mods.keys()) {
		const auto mod = mods[index];
		m_pixels[index] += mod * inversion;
	}
	notifyUpdate();
}

void SheetData::setPixel(int index, int color) {
	m_pixels[index] = color;
}

void SheetData::notifyUpdate() {
	emit pixelsChanged();
	emit changeOccurred();
}

std::unique_ptr<NostalgiaGraphic> SheetData::toNostalgiaGraphic() const {
	auto ng = std::make_unique<NostalgiaGraphic>();
	const auto highestColorIdx = static_cast<uint8_t>(*std::max_element(m_pixels.begin(), m_pixels.end()));
	ng->defaultPalette = m_currentPalettePath.toUtf8().data();
	ng->bpp = highestColorIdx > 15 ? 8 : 4;
	ng->columns = m_columns;
	ng->rows = m_rows;
	auto pixelCount = static_cast<std::size_t>(ng->rows * ng->columns * PixelsPerTile);
	if (ng->bpp == 4) {
		ng->pixels.resize(pixelCount / 2);
		for (std::size_t i = 0; i < pixelCount && i < static_cast<std::size_t>(m_pixels.size()); ++i) {
			if (i & 1) {
				ng->pixels[i / 2] |= static_cast<uint8_t>(m_pixels[static_cast<std::intptr_t>(i)]) << 4;
			} else {
				ng->pixels[i / 2] = static_cast<uint8_t>(m_pixels[static_cast<std::intptr_t>(i)]);
			}
		}
	} else {
		ng->pixels.resize(pixelCount);
		for (std::size_t i = 0; i < ng->pixels.size(); ++i) {
			ng->pixels[i] = static_cast<uint8_t>(m_pixels[static_cast<std::intptr_t>(i)]);
		}
	}
	return ng;
}

int SheetData::activeTool() const {
	return static_cast<int>(m_activeTool);
}

bool SheetData::clipboardEmpty() const {
	return m_clipboard.empty();
}

void SheetData::cutToClipboard() {
	m_clipboard.setPoints(m_selectionStart, m_selectionEnd);
	cutToClipboard(&m_clipboard);
}

void SheetData::cutToClipboard(TileSheetClipboard *cb) {
	const auto pt1 = cb->point1();
	const auto pt2 = cb->point2();
	auto apply = std::make_unique<TileSheetClipboard>();
	for (int x = pt1.x; x <= pt2.x; ++x) {
		for (int y = pt1.y; y <= pt2.y; ++y) {
			const auto pt = common::Point(x, y);
			const auto i = ptToIdx(pt, m_columns);
			const auto s = m_pixelSelected[i];
			if (s) {
				cb->addPixel(m_pixels[i]);
				apply->addPixel(0);
			}
		}
	}
	apply->setPoints(cb->point1(), cb->point2());
	auto restore = std::make_unique<TileSheetClipboard>(*cb);
	m_cmdStack->push(new PasteClipboardCommand(this, std::move(restore), std::move(apply)));
}

void SheetData::copyToClipboard() {
	m_clipboard.setPoints(m_selectionStart, m_selectionEnd);
	copyToClipboard(&m_clipboard);
}

void SheetData::copyToClipboard(TileSheetClipboard *cb) {
	const auto p1 = cb->point1();
	const auto p2 = cb->point2();
	for (int x = p1.x; x <= p2.x; ++x) {
		for (int y = p1.y; y <= p2.y; ++y) {
			const auto pt = common::Point(x, y);
			const auto idx = ptToIdx(pt, m_columns);
			cb->addPixel(m_pixels[idx]);
		}
	}
}

void SheetData::pasteClipboard() {
	auto apply = std::make_unique<TileSheetClipboard>(m_clipboard);
	auto restore = std::make_unique<TileSheetClipboard>();
	const auto p2 = m_selectionStart + (apply->point2() - apply->point1());
	apply->setPoints(m_selectionStart, p2);
	restore->setPoints(m_selectionStart, p2);
	markSelection(p2);
	copyToClipboard(restore.get());
	m_cmdStack->push(new PasteClipboardCommand(this, std::move(restore), std::move(apply)));
}

void SheetData::applyClipboard(const TileSheetClipboard &cb) {
	cb.pastePixels(cb.point1(), &m_pixels, m_columns);
	notifyUpdate();
}

void SheetData::markSelection(common::Point selectionEnd) {
	// mark selection
	m_selectionEnd = selectionEnd;
	ox::memsetElements(m_pixelSelected.data(), 0, static_cast<std::size_t>(m_pixelSelected.size()));
	for (auto x = m_selectionStart.x; x <= m_selectionEnd.x; ++x) {
		for (auto y = m_selectionStart.y; y <= m_selectionEnd.y; ++y) {
			m_pixelSelected[ptToIdx(x, y, m_columns)] |= 1;
		}
	}
	emit pixelSelectedChanged((m_selectionEnd.x - m_selectionStart.x + 1) * (m_selectionEnd.y - m_selectionStart.y + 1));
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

void SheetData::setActiveTool(int t) {
	m_activeTool = static_cast<TileSheetTool>(t);
	ox::memsetElements(m_pixelSelected.data(), 0, static_cast<std::size_t>(m_pixelSelected.size()));
	emit pixelSelectedChanged(0);
}

void SheetData::updatePixels(const NostalgiaGraphic *ng) {
	m_pixels.clear();
	m_pixelSelected.clear();
	if (ng->bpp == 8) {
		for (std::size_t i = 0; i < ng->pixels.size(); i++) {
			m_pixels.push_back(ng->pixels[i]);
			m_pixelSelected.push_back(0);
		}
	} else {
		for (std::size_t i = 0; i < ng->pixels.size() * 2; i++) {
			uint8_t p;
			if (i & 1) {
				p = ng->pixels[i / 2] >> 4;
			} else {
				p = ng->pixels[i / 2] & 0xF;
			}
			m_pixels.push_back(p);
			m_pixelSelected.push_back(0);
		}
	}
	emit pixelsChanged();
}

const char *SheetData::activeToolString() const {
	return toString(m_activeTool);
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
	auto updateAfterBtn = new QPushButton(tr("&Mod GTE"), tb);
	m_toolBtns.addButton(new QPushButton(tr("&Select"), tb), static_cast<int>(TileSheetTool::Select));
	m_toolBtns.addButton(new QPushButton(tr("&Draw"), tb), static_cast<int>(TileSheetTool::Draw));
	m_toolBtns.addButton(new QPushButton(tr("F&ill"), tb), static_cast<int>(TileSheetTool::Fill));
	tb->addWidget(m_tilesX);
	tb->addWidget(m_tilesY);
	tb->addSeparator();
	tb->addWidget(updateAfterBtn);
	tb->addSeparator();
	for (auto btn : m_toolBtns.buttons()) {
		btn->setCheckable(true);
		tb->addWidget(btn);
	}
	m_toolBtns.button(m_sheetData.activeTool())->setChecked(true);
	connect(&m_toolBtns, &QButtonGroup::idClicked, &m_sheetData, &SheetData::setActiveTool);
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
	connect(updateAfterBtn, &QPushButton::clicked, this, &TileSheetEditor::updateAfterClicked);
	m_canvas->rootContext()->setContextProperty("sheetData", &m_sheetData);
	m_canvas->setSource(QUrl::fromLocalFile(":/qml/TileSheetEditor.qml"));
	m_canvas->setResizeMode(QQuickWidget::SizeRootObjectToView);
	setPalette();
	setColorTable();
	restoreState();
	connect(&m_sheetData, &SheetData::changeOccurred, [this] { setUnsavedChanges(true); });
	connect(&m_sheetData, &SheetData::paletteChanged, this, &TileSheetEditor::setColorTable);
	connect(&m_sheetData, &SheetData::pixelSelectedChanged, [this](int selected) {
		setCutEnabled(selected);
		setCopyEnabled(selected);
		setPasteEnabled(selected && !m_sheetData.clipboardEmpty());
	});
	setExportable(true);
	installEventFilter(this);
}

TileSheetEditor::~TileSheetEditor() {
	saveState();
}

QString TileSheetEditor::itemName() const {
	return m_itemName;
}

void TileSheetEditor::exportFile() {
	auto path = QFileDialog::getSaveFileName(this, tr("Export to Image"), "",
	                                         tr("PNG (*.png)"));
	if (path != "") {
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
}

void TileSheetEditor::cut() {
	m_sheetData.cutToClipboard();
}

void TileSheetEditor::copy() {
	m_sheetData.copyToClipboard();
}

void TileSheetEditor::paste() {
	m_sheetData.pasteClipboard();
}

void TileSheetEditor::saveItem() {
	m_sheetData.save(m_ctx, m_itemPath);
}

bool TileSheetEditor::eventFilter(QObject *o, QEvent *e) {
	if (e->type() == QEvent::KeyPress) {
		const auto k = static_cast<QKeyEvent*>(e)->key();
		const auto max = m_colorPicker.colorTable->rowCount() - 1;
		if (k >= Qt::Key_0 && k <= Qt::Key_9) {
			const auto val = k - Qt::Key_1 + (k == Qt::Key_0 ? 10 : 0);
			m_colorPicker.colorTable->setCurrentCell(std::min(max, val), 0);
			return true;
		}
	}
	return QWidget::eventFilter(o, e);
}

QWidget *TileSheetEditor::setupColorPicker(QWidget *parent) {
	auto colorPicker = new QWidget(parent);
	auto lyt = new QVBoxLayout(colorPicker);
	m_colorPicker.palette = new QComboBox(colorPicker);
	m_colorPicker.colorTable = new QTableWidget(colorPicker);
	m_colorPicker.colorTable->setItemDelegate(&m_colorTableDelegate);
	m_colorPicker.colorTable->setColumnCount(2);
	m_colorPicker.colorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_colorPicker.colorTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_colorPicker.colorTable->setHorizontalHeaderLabels(QStringList() << tr("Hex Code") << tr("Color"));
	m_colorPicker.colorTable->horizontalHeader()->setStretchLastSection(true);
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
	m_ctx->project->subscribe(studio::ProjectEvent::FileUpdated, m_colorPicker.palette, [this](QString path) {
		if (path == m_sheetData.palettePath()) {
			m_sheetData.reload(m_ctx);
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
	QSettings settings(m_ctx->orgName, ModuleName);
	settings.beginGroup("TileSheetEditor/" + m_itemName);
	settings.setValue("m_splitter/state", m_splitter->saveState());
	settings.setValue("m_sheetData/tileRows", m_sheetData.rows());
	settings.setValue("m_sheetData/tileColumns", m_sheetData.columns());
	settings.setValue("m_colorPicker.colorTable/geometry", m_colorPicker.colorTable->horizontalHeader()->saveState());
	settings.endGroup();
}

void TileSheetEditor::restoreState() {
	QSettings settings(m_ctx->orgName, ModuleName);
	settings.beginGroup("TileSheetEditor/" + m_itemName);
	m_splitter->restoreState(settings.value("m_splitter/state", m_splitter->saveState()).toByteArray());
	m_colorPicker.colorTable->horizontalHeader()->restoreState(settings.value("m_colorPicker.colorTable/geometry", m_colorPicker.colorTable->horizontalHeader()->saveState()).toByteArray());
	settings.endGroup();
}

QString TileSheetEditor::paletteName(QString palettePath) const {
	return studio::filePathToName(palettePath, PaletteDir, FileExt_npal);
}

QString TileSheetEditor::palettePath(QString paletteName) const {
	return PaletteDir + paletteName + FileExt_npal;
}

QImage TileSheetEditor::toQImage(NostalgiaGraphic *ng, NostalgiaPalette *npal) const {
	const auto w = ng->columns * TileWidth;
	const auto h = ng->rows * TileHeight;
	QImage dst(w, h, QImage::Format_RGB32);
	auto setPixel = [&dst, npal, ng](std::size_t i, uint8_t p) {
		const auto color = toColor32(npal->colors[p]) >> 8;
		const auto pt = idxToPt(i, ng->columns);
		dst.setPixel(pt.x, pt.y, color);
	};
	if (ng->bpp == 4) {
		for (std::size_t i = 0; i < ng->pixels.size(); ++i) {
			auto p1 = ng->pixels[i] & 0xF;
			auto p2 = ng->pixels[i] >> 4;
			setPixel(i * 2 + 0, p1);
			setPixel(i * 2 + 1, p2);
		}
	} else {
		for (std::size_t i = 0; i < ng->pixels.size(); i++) {
			const auto p = ng->pixels[i];
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
		auto hi = tbl->item(i, 0);
		hi->setFlags(hi->flags() & ~Qt::ItemIsEditable);
		auto color = new QTableWidgetItem;
		color->setText(hexColors[i]);
		tbl->setItem(i, 1, color);
		auto ci = tbl->item(i, 1);
		ci->setFlags(ci->flags() & ~Qt::ItemIsEditable);
	}
}

void TileSheetEditor::updateAfterClicked() {
	auto d = new ModAfterDialog(m_sheetData.palette(), this);
	connect(d, &ModAfterDialog::accepted, [this, d] {
		auto mod = d->mod();
		if (mod) {
			m_sheetData.modGteCmd(d->color(), mod);
		}
	});
	connect(d, &ModAfterDialog::finished, d, &ModAfterDialog::deleteLater);
	d->open();
}

}
