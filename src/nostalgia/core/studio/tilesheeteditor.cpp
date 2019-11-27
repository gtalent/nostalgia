/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <QQmlContext>
#include <QQuickWidget>
#include <QSettings>
#include <QSplitter>
#include <QVBoxLayout>

#include "consts.hpp"
#include "tilesheeteditor.hpp"

namespace nostalgia::core {

[[nodiscard]]
QVector<Color32> toPixels(const NostalgiaGraphic *ng, const NostalgiaPalette *npal) {
	if (!npal) {
		npal = &ng->pal;
	}

	QVector<Color32> out;
	out.reserve(ng->tiles.size() * sizeof(Color16));

	if (ng->bpp == 8) {
		for (std::size_t i = 0; i < ng->tiles.size(); i++) {
			auto p = ng->tiles[i];
			auto c = p < npal->colors.size() ? npal->colors[p] : 0;
			out.push_back(toColor32(c));
		}
	} else {
		for (std::size_t i = 0; i < ng->tiles.size() * 2; i++) {
			uint8_t p;
			if (i & 1) {
				p = ng->tiles[i / 2] >> 4;
			} else {
				p = ng->tiles[i / 2] & 0xF;
			}
			auto c = p < npal->colors.size() ? npal->colors[p] : 0;
			out.push_back(toColor32(c));
		}
	}

	return out;
}

[[nodiscard]]
QVector<Color32> toPixels(const studio::Context *ctx, QString ngPath, QString palPath = "") {
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
	return toPixels(ng.get(), npal.get());
}


QString SheetData::pixel(int index) {
	return "#" + QString("%1").arg(QString::number(m_pixels[index], 16), 8, '0');
}

void SheetData::updatePixels(const studio::Context *ctx, QString path) {
	m_pixels = toPixels(ctx, path);
}


TileSheetEditor::TileSheetEditor(QString path, const studio::Context *ctx, QWidget *parent): QWidget(parent) {
	m_ctx = ctx;
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
	restoreState();
}

TileSheetEditor::~TileSheetEditor() {
	saveState();
}

QWidget *TileSheetEditor::setupColorPicker(QWidget *parent) {
	auto colorPicker = new QWidget(parent);
	auto lyt = new QVBoxLayout(colorPicker);
	m_colorPicker.palette = new QComboBox(colorPicker);
	m_colorPicker.colorTable = new QTableWidget(colorPicker);
	lyt->addWidget(m_colorPicker.palette);
	lyt->addWidget(m_colorPicker.colorTable);
	return colorPicker;
}

void TileSheetEditor::saveState() {
	QSettings settings(m_ctx->orgName, PluginName);
	settings.beginGroup("TileSheetEditor/state");
	settings.setValue("m_splitter/state", m_splitter->saveState());
	settings.endGroup();
}

void TileSheetEditor::restoreState() {
	QSettings settings(m_ctx->orgName, PluginName);
	settings.beginGroup("TileSheetEditor/state");
	m_splitter->restoreState(settings.value("m_splitter/state", m_splitter->saveState()).toByteArray());
	settings.endGroup();
}

}
