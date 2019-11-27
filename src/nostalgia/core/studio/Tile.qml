
import QtQuick 2.0

Rectangle {
	id: tile;
	property int tileNumber: -1
	width: parent.width
	height: parent.height
	x: parent.width / 2 - tile.width / 2
	y: parent.height / 2 - tile.height / 2
	color: '#000000'

	Grid {
		width: tile.width
		height: tile.height
		rows: 8
		columns: 8
		Repeater {
			model: 64
			Pixel {
				pixelNumber: index + 64 * tile.tileNumber
			}
		}
	}

	// place an outline Rectangle above the pixels
	Rectangle {
		width: parent.width
		height: parent.height
		// make fill transparent
		color: '#00000000'
		border.color: '#000000'
		border.width: 2
	}

}
