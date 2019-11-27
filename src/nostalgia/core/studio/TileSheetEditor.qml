
import QtQuick 2.0
import 'qrc:/qml/'

Rectangle {
	id: tileSheetEditor
	color: '#717d7e'

	Grid {
		id: tileGrid
		property int baseTileSize: Math.min(parent.width / tileGrid.columns, parent.height / tileGrid.rows)
		width: tileGrid.columns * tileGrid.baseTileSize * 0.85
		height: tileGrid.rows * tileGrid.baseTileSize * 0.85
		anchors.horizontalCenter: tileSheetEditor.horizontalCenter
		anchors.verticalCenter: tileSheetEditor.verticalCenter
		rows: 2
		columns: 2
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
