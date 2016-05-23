import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0

PixelPerfectItem {
    property int internalHeight
    height: parent.height
    width: parent.width
    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.porcelain
    }
}

