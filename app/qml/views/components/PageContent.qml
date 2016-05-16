import QtQuick 2.4
import "../models"
import "../../constants"

Item {

    anchors.fill: parent
    default property alias content: c.data

    Item {
        id: c
        anchors.fill: parent
        // PAGE CONTENT
    }
}

