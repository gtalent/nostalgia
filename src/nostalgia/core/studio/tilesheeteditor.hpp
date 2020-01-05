/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QQuickItem>
#include <QSplitter>
#include <QStringList>
#include <QStringView>
#include <QTableWidget>
#include <QUndoStack>
#include <QVariant>
#include <QWidget>

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
		QQuickItem *m_prevPixelUpdated = nullptr;
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

		int columns();

		int rows();

		const QVector<int> &pixels();

		QStringList palette();

		void updatePixels(const studio::Context *ctx, QString ngPath, QString palPath = "");

		void setSelectedColor(int index);

		QUndoStack *undoStack();

	public slots:
		void setColumns(int columns);

		void setRows(int rows);

	private:
		void updatePixels(const NostalgiaGraphic *ng, const NostalgiaPalette *npal);

	signals:
		void columnsChanged(int);

		void rowsChanged(int);

		void pixelsChanged();

		void paletteChanged();

};


class TileSheetEditor: public studio::Editor {
	Q_OBJECT

	private:
		QString m_itemName;
		const studio::Context *m_ctx = nullptr;
		SheetData m_sheetData;
		QSplitter *m_splitter = nullptr;
		class LabeledSpinner *m_tilesX = nullptr;
		class LabeledSpinner *m_tilesY = nullptr;
		struct {
			QComboBox *palette = nullptr;
			QTableWidget *colorTable = nullptr;
		} m_colorPicker;

	public:
		TileSheetEditor(QString path, const studio::Context *ctx, QWidget *parent);

		virtual ~TileSheetEditor();

		QString itemName() override;

		void save() override;

		QUndoStack *undoStack() override;

	private:
		QWidget *setupToolBar();

		QWidget *setupColorPicker(QWidget *widget);

		void setColorTable(QStringList hexColors);

		void saveState();

		void restoreState();

	public slots:
		void colorSelected();

};

}
