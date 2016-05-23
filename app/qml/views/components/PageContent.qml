import QtQuick 2.4
import Dekko.Components 1.0
import "../models"
import "../../constants"

PixelPerfectItem {

    anchors.fill: parent
    default property alias content: c.data

    PixelPerfectItem {
        id: c
        anchors.fill: parent
        // PAGE CONTENT
    }
}

