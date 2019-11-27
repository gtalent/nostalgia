
import QtQuick 2.0

Rectangle {
	id: pixel;
	property int pixelNumber: index
	color: sheetData.pixel(pixelNumber)
	width: parent.width / 8
	height: parent.height / 8
	border.color: '#717d7e'
	border.width: 1

	Text {
		text: (index % 8 + 1) + ', ' + Math.floor(index / 8 + 1)
		font.family: 'Helvetica'
		font.pointSize: 11
		color: '#717d7e88'
		visible: pixel.width > 42 // that's acutally not HGtG reference
		anchors.horizontalCenter: pixel.horizontalCenter
		anchors.bottom: pixel.bottom
	}
}
