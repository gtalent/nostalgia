/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.0
import 'qrc:/qml/'

Rectangle {
	id: tileSheetEditor
	color: '#717d7e'

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		acceptedButtons: Qt.LeftButton
		onPositionChanged: {
			var gridX = mouseX - tileGrid.x;
			var gridY = mouseY - tileGrid.y;
			var tile = tileGrid.childAt(gridX, gridY);
			if (tile === null) {
				return;
			}
			var tileX = gridX - tile.x;
			var tileY = gridY - tile.y;
			var pixel = tile.pixelAt(tileX, tileY);
			if (pixel === null) {
				return;
			}
			sheetData.updatePixels([pixel]);
		}

		Grid {
			id: tileGrid
			property int baseTileSize: Math.min(parent.width / tileGrid.columns, parent.height / tileGrid.rows)
			width: tileGrid.columns * tileGrid.baseTileSize * 0.90
			height: tileGrid.rows * tileGrid.baseTileSize * 0.90
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			rows: sheetData.rows
			columns: sheetData.columns
			Repeater {
				model: tileGrid.rows * tileGrid.columns
				Tile {
					tileNumber: index
					width: tileGrid.width / tileGrid.columns
					height: tileGrid.height / tileGrid.rows
				}
			}
		}
	}

}
