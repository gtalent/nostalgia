/*
 * Copyright 2016 - 2019 gtalent2@gmail.com
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.0
import QtQuick.Controls 2.14
import 'qrc:/qml/'

ScrollView {
	id: tileSheetEditor
	ScrollBar.horizontal.policy: ScrollBar.AsNeeded
	ScrollBar.vertical.policy: ScrollBar.AsNeeded
	contentWidth: tileGrid.width
	contentHeight: tileGrid.height
	clip: true
	focusPolicy: Qt.WheelFocus

	background: Rectangle {
		color: '#717d7e'
	}

	MouseArea {
		id: mouseArea
		width: tileSheetEditor.width
		height: tileSheetEditor.height
		acceptedButtons: Qt.LeftButton

		onPressed: {
			var pixel = pixelAt(mouseX, mouseY);
			if (pixel) {
				sheetData.beginCmd()
				sheetData.updatePixel(pixel)
			}
		}

		onWheel: {
			if (wheel.modifiers & Qt.ControlModifier) {
				const mod = tileGrid.scaleFactor / 10;
				if (wheel.angleDelta.y > 0 && tileGrid.scaleFactor < 7) {
					tileGrid.scaleFactor += mod;
				} else if (tileGrid.scaleFactor > 0.9) {
					tileGrid.scaleFactor -= mod;
				}
				wheel.accepted = true;
			} else {
				wheel.accepted = false;
			}
		}

		onPositionChanged: sheetData.updatePixel(pixelAt(mouseX, mouseY))
		onReleased: sheetData.endCmd()
		onCanceled: sheetData.endCmd()

		function pixelAt(x, y) {
			var gridX = x - tileGrid.x;
			var gridY = y - tileGrid.y;
			var tile = tileGrid.childAt(gridX, gridY);
			if (tile === null) {
				return null;
			}
			var tileX = gridX - tile.x;
			var tileY = gridY - tile.y;
			var pixel = tile.pixelAt(tileX, tileY);
			return pixel;
		}

	}

	Grid {
		id: tileGrid
		property double scaleFactor: 0.9
		property int baseTileSize: Math.min(2000 / tileGrid.columns, 1000 / tileGrid.rows)
		width: tileGrid.columns * tileGrid.baseTileSize * tileGrid.scaleFactor
		height: tileGrid.rows * tileGrid.baseTileSize * tileGrid.scaleFactor
		//anchors.horizontalCenter: parent.horizontalCenter
		//anchors.verticalCenter: parent.verticalCenter
		rows: sheetData ? sheetData.rows : 1
		columns: sheetData ? sheetData.columns : 1
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
