import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0
import "../components"

DekkoPage {
    id: nowtSelected

    extendHeader: true


    CachedImage {
        id: image
        height: units.gu(20)
        width: height
        anchors.centerIn: parent
        name: Icons.InboxIcon
        color: UbuntuColors.ash
    }

    Label {
        fontSize: "large"
        text: qsTr("No message selected")
        anchors {
            top: image.bottom
            topMargin: units.gu(2)
            horizontalCenter: parent.horizontalCenter
        }
    }
}

