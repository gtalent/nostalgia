/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QStringList>
#include <QUndoStack>
#include <QVariant>

#include <nostalgia/core/gfx.hpp>
#include <nostalgia/studio/studio.hpp>

namespace nostalgia::core {

class SheetData: public QObject {
	Q_OBJECT
	Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY columnsChanged)
	Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
	Q_PROPERTY(QVector<int> pixels READ pixels NOTIFY pixelsChanged)
	Q_PROPERTY(QStringList palette READ palette NOTIFY paletteChanged)

	private:
		class QQuickItem *m_prevPixelUpdated = nullptr;
		QString m_tilesheetPath;
		QString m_currentPalettePath;
		uint64_t m_cmdIdx = 0;
		QUndoStack m_cmdStack;
		QStringList m_palette;
		QVector<int> m_pixels;
		int m_columns = 1;
		int m_rows = 1;
		int m_selectedColor = 0;

	public:
		Q_INVOKABLE void updatePixel(QVariant pixel);

		Q_INVOKABLE void beginCmd();

		Q_INVOKABLE void endCmd();

		[[nodiscard]] int columns() const;

		[[nodiscard]] int rows() const;

		const QVector<int> &pixels() const;

		[[nodiscard]] QStringList palette() const;

		[[nodiscard]] QString palettePath() const;

		void load(const studio::Context *ctx, QString ngPath, QString palPath = "");

		void save(const studio::Context *ctx, QString ngPath) const;

		void setPalette(const NostalgiaPalette *pal);

		void setPalette(const studio::Context *ctx, QString palPath);

		void setSelectedColor(int index);

		QUndoStack *undoStack();

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

	private:
		void updatePixels(const NostalgiaGraphic *ng);

		[[nodiscard]] std::unique_ptr<NostalgiaGraphic> toNostalgiaGraphic() const;

	signals:
		void changeOccurred();

		void columnsChanged(int);

		void rowsChanged(int);

		void pixelsChanged();

		void paletteChanged();

};


class TileSheetEditor: public studio::Editor {
	Q_OBJECT

	private:
		QString m_itemPath;
		QString m_itemName;
		const studio::Context *m_ctx = nullptr;
		SheetData m_sheetData;
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

		QString itemName() override;

		QUndoStack *undoStack() override;

	protected:
		void saveItem() override;

	private:
		QWidget *setupColorPicker(QWidget *widget);

		void setPalette();

		void saveState();

		void restoreState();

		[[nodiscard]] QString paletteName(QString palettePath) const;

		[[nodiscard]] QString palettePath(QString palettePath) const;

	public slots:
		void colorSelected();

		void setColorTable();

};

}
