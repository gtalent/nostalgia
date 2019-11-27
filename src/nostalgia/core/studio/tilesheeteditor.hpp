/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <QSplitter>
#include <QStringList>
#include <QStringView>
#include <QTableWidget>
#include <QWidget>

#include <nostalgia/core/gfx.hpp>
#include <nostalgia/studio/studio.hpp>

namespace nostalgia::core {

class SheetData: public QObject {
	Q_OBJECT

	private:
		QStringList m_palette;
		QVector<uint8_t> m_pixels;

	public:
		Q_INVOKABLE QString pixel(int index);

		void updatePixels(const NostalgiaGraphic *ng, const NostalgiaPalette *npal);

		void updatePixels(const studio::Context *ctx, QString ngPath, QString palPath = "");

	signals:
		void refreshTileSheet();

};


class TileSheetEditor: public QWidget {
	Q_OBJECT

	private:
		const studio::Context *m_ctx = nullptr;
		SheetData m_sheetData;
		QSplitter *m_splitter = nullptr;
		struct {
			QComboBox *palette = nullptr;
			QTableWidget *colorTable = nullptr;
		} m_colorPicker;

	public:
		TileSheetEditor(QString path, const studio::Context *ctx, QWidget *parent);

		virtual ~TileSheetEditor();

	private:
		QWidget *setupColorPicker(QWidget *widget);

		void saveState();

		void restoreState();

};

}
