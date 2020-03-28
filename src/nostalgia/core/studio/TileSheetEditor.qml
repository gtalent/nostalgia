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

Rectangle {
	id: tileSheetEditor
	clip: true
	color: '#717d7e'

	MouseArea {
		id: mouseArea
		width: tileSheetEditor.width
		height: tileSheetEditor.height
		acceptedButtons: Qt.LeftButton | Qt.RightButton

		onClicked: {
			if (mouse.button === Qt.RightButton) {
				contextMenu.popup();
			} else {
				contextMenu.dismiss();
			}
		}

		onPressed: {
			if (mouse.button === Qt.LeftButton && !contextMenu.visible) {
				var pixel = pixelAt(mouseX, mouseY);
				if (pixel) {
					sheetData.beginCmd();
					sheetData.updatePixel(pixel);
				}
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


				if (tileGrid.width <= this.width) {
					tileGrid.x = this.width / 2 - tileGrid.width / 2;
				}
				if (tileGrid.height <= this.height) {
					tileGrid.y = this.height / 2 - tileGrid.height / 2;
				}
			} else {
				const mod = 15;
				if (tileGrid.width > this.width) {
					if (wheel.angleDelta.x > 0) {
						if (tileGrid.x < tileGrid.width / 2) {
							tileGrid.x += mod;
						}
					} else if (wheel.angleDelta.x < 0) {
						let x2 = tileGrid.x + tileGrid.width;
						if (x2 > this.width / 2) {
							tileGrid.x -= mod;
						}
					}
				}
				if (tileGrid.height > this.height) {
					if (wheel.angleDelta.y > 0) {
						if (tileGrid.y < this.height / 2) {
							tileGrid.y += mod;
						}
					} else if (wheel.angleDelta.y < 0) {
						let y2 = tileGrid.y + tileGrid.height;
						if (y2 > this.height / 2) {
							tileGrid.y -= mod;
						}
					}
				}
			}
			wheel.accepted = true;
		}

		Menu {
			id: contextMenu

			MenuItem {
				text: "Insert Tile"
				onTriggered: {
					var tile = mouseArea.tileAt(contextMenu.x, contextMenu.y);
					sheetData.insertTileCmd(tile.tileNumber);
				}
			}

			MenuItem {
				text: "Delete Tile"
				onTriggered: {
					var tile = mouseArea.tileAt(contextMenu.x, contextMenu.y);
					sheetData.deleteTileCmd(tile.tileNumber);
				}
			}
		}

		onPositionChanged: {
			if (mouseArea.pressedButtons & Qt.LeftButton && !contextMenu.visible) {
				sheetData.updatePixel(pixelAt(mouseX, mouseY));
			}
		}

		onReleased: sheetData.endCmd()
		onCanceled: sheetData.endCmd()

		function tileAt(x, y) {
			var gridX = x - tileGrid.x;
			var gridY = y - tileGrid.y;
			return tileGrid.childAt(gridX, gridY);
		}

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
		x: tileSheetEditor.width / 2 - this.width / 2
		y: tileSheetEditor.height / 2 - this.height / 2
		width: tileGrid.columns * tileGrid.baseTileSize * tileGrid.scaleFactor
		height: tileGrid.rows * tileGrid.baseTileSize * tileGrid.scaleFactor
		rows: sheetData ? sheetData.rows : 1
		columns: sheetData ? sheetData.columns : 1
		states: State {
			name: "widthChanged"
			PropertyChanges { target: tileGrid.width; width: tileGrid.columns * tileGrid.baseTileSize * tileGrid.scaleFactor }
		}
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
