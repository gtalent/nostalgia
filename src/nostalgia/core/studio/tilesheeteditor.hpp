/*
 * Copyright 2016 - 2020 gary@drinkingtea.net
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QButtonGroup>
#include <QStringList>
#include <QStyledItemDelegate>
#include <QUndoStack>
#include <QVariant>

#include <nostalgia/common/bounds.hpp>
#include <nostalgia/core/gfx.hpp>
#include <nostalgia/studio/studio.hpp>

namespace nostalgia::core {

// Command IDs to use with QUndoCommand::id()
enum class CommandId {
	UpdatePixel = 1,
	ModPixel = 2,
	UpdateDimension = 3,
	InsertTile = 4,
	ClipboardPaste = 4,
};

enum class TileSheetTool: int {
	Select,
	Draw,
	Fill,
};

[[nodiscard]] constexpr auto toString(TileSheetTool t) noexcept {
	switch (t) {
		case TileSheetTool::Select:
			return "Select";
		case TileSheetTool::Draw:
			return "Draw";
		case TileSheetTool::Fill:
			return "Fill";
	}
	return "";
}

struct PixelChunk {
	static constexpr auto TypeName = "net.drinkingtea.nostalgia.core.studio.PixelChunk";
	static constexpr auto Fields = 2;
	static constexpr auto TypeVersion = 1;
	common::Point pt;
	int size = 0;
};

template<typename T>
ox::Error model(T *io, PixelChunk *c) {
	io->template setTypeInfo<PixelChunk>();
	oxReturnError(io->field("pt", &c->pt));
	oxReturnError(io->field("size", &c->size));
	return OxError(0);
}

struct TileSheetClipboard {
	static constexpr auto TypeName = "net.drinkingtea.nostalgia.core.studio.TileSheetClipboard";
	static constexpr auto Fields = 3;
	static constexpr auto TypeVersion = 1;

	template<typename T>
	friend ox::Error model(T*, TileSheetClipboard*);

	protected:
		std::vector<int> m_pixels;
		common::Point m_p1;
		common::Point m_p2;

	public:
		void addPixel(int color);

		void clear();

		[[nodiscard]] bool empty() const;

		void pastePixels(common::Point pt, QVector<int> *tgt, int tgtColumns) const;

		void setPoints(common::Point p1, common::Point p2);

		[[nodiscard]] common::Point point1() const;

		[[nodiscard]] common::Point point2() const;

};

template<typename T>
ox::Error model(T *io, TileSheetClipboard *b) {
	io->template setTypeInfo<TileSheetClipboard>();
	oxReturnError(io->field("pixels", &b->m_pixels));
	oxReturnError(io->field("p1", &b->m_p1));
	oxReturnError(io->field("p2", &b->m_p2));
	return OxError(0);
}

struct TileSheetEditorColorTableDelegate: public QStyledItemDelegate {

	void paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &idx) const;

};

class SheetData: public QObject {
	Q_OBJECT
	Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
	Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
	Q_PROPERTY(QVector<int> pixelSelected READ pixelSelected NOTIFY pixelSelectedChanged)
	Q_PROPERTY(QVector<int> pixels READ pixels NOTIFY pixelsChanged)
	Q_PROPERTY(QStringList palette READ palette NOTIFY paletteChanged)
	Q_PROPERTY(QString activeTool READ activeToolString)

	private:
		class QQuickItem *m_prevPixelOperand = nullptr;
		QString m_tilesheetPath;
		QString m_currentPalettePath;
		uint64_t m_cmdIdx = 0;
		QUndoStack *m_cmdStack;
		QStringList m_palette;
		QVector<int> m_pixelSelected;
		QVector<int> m_pixels;
		common::Point m_selectionStart = {-1, -1};
		common::Point m_selectionEnd = {-1, -1};
		int m_columns = 1;
		int m_rows = 1;
		int m_selectedColor = 0;
		TileSheetTool m_activeTool = TileSheetTool::Draw;
		TileSheetClipboard m_clipboard;

	public:
		SheetData(QUndoStack*);

		Q_INVOKABLE void selectPixel(QVariant pixel);

		Q_INVOKABLE void updatePixel(QVariant pixel);

		Q_INVOKABLE void beginCmd();

		Q_INVOKABLE void endCmd();

		Q_INVOKABLE void insertTileCmd(int tileIdx);

		Q_INVOKABLE void deleteTileCmd(int tileIdx);

		[[nodiscard]] int columns() const;

		[[nodiscard]] int rows() const;

		[[nodiscard]] const QVector<int> &pixelSelected() const;

		[[nodiscard]] const QVector<int> &pixels() const;

		[[nodiscard]] QStringList palette() const;

		[[nodiscard]] QString palettePath() const;

		void load(const studio::Context *ctx, QString ngPath, QString palPath = "");

		void reload(const studio::Context *ctx);

		void save(const studio::Context *ctx, QString ngPath) const;

		void setPalette(const NostalgiaPalette *pal);

		void setPalette(const studio::Context *ctx, QString palPath);

		void insertTile(int tileIdx, QVector<int> tileData = {});

		QVector<int> deleteTile(int tileIdx);

		void setSelectedColor(int index);

		void modGteCmd(int val, int mod);

		void modPixels(const QHash<int, int> &pixels, int inversion);

		[[nodiscard]] std::unique_ptr<NostalgiaGraphic> toNostalgiaGraphic() const;

		[[nodiscard]] int activeTool() const;

		[[nodiscard]] bool clipboardEmpty() const;

		void cutToClipboard();

		void cutToClipboard(TileSheetClipboard *cb);

		void copyToClipboard();

		void copyToClipboard(TileSheetClipboard *cb);

		void pasteClipboard();

		void applyClipboard(const TileSheetClipboard &cb);

		void markSelection(common::Point selectionEnd);

	public slots:
		void setColumns(int columns);

		void setRows(int rows);

		/**
		 * Sets columns through a QUndoCommand.
		 */
		void updateColumns(int columns);

		/**
		 * Sets rows through a QUndoCommand.
		 */
		void updateRows(int rows);

		void setActiveTool(int t);

	private:
		void updatePixels(const NostalgiaGraphic *ng);

		const char *activeToolString() const;

	signals:
		void changeOccurred();

		void columnsChanged(int);

		void rowsChanged(int);

		void pixelSelectedChanged(int pixelsSelected);

		void pixelsChanged();

		void paletteChanged();

};


class TileSheetEditor: public studio::Editor {
	Q_OBJECT

	private:
		TileSheetEditorColorTableDelegate m_colorTableDelegate;
		QString m_itemPath;
		QString m_itemName;
		const studio::Context *m_ctx = nullptr;
		SheetData m_sheetData;
		QButtonGroup m_toolBtns;
		class QSplitter *m_splitter = nullptr;
		struct LabeledSpinner *m_tilesX = nullptr;
		struct LabeledSpinner *m_tilesY = nullptr;
		class QQuickWidget* m_canvas = nullptr;
		struct {
			QComboBox *palette = nullptr;
			class QTableWidget *colorTable = nullptr;
		} m_colorPicker;

	public:
		TileSheetEditor(QString path, const studio::Context *ctx, QWidget *parent);

		virtual ~TileSheetEditor();

		QString itemName() const override;

		void exportFile() override;

		void cut() override;

		void copy() override;

		void paste() override;

	protected:
		void saveItem() override;

	private:
		QWidget *setupColorPicker(QWidget *widget);

		void setPalette();

		void saveState();

		void restoreState();

		[[nodiscard]] QString paletteName(QString palettePath) const;

		[[nodiscard]] QString palettePath(QString palettePath) const;

		[[nodiscard]] QImage toQImage(NostalgiaGraphic *ng, NostalgiaPalette *npal) const;

	public slots:
		void colorSelected();

		void setColorTable();

	private slots:
		void updateAfterClicked();

};

}
