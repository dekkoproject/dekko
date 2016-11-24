import QtQuick 2.4
import Ubuntu.Components 1.3
import "../src"

NotificationBase {
    id: base

    Rectangle {
        anchors.centerIn: parent
        height: units.gu(20)
        width: height
        radius: height / 2
        color: UbuntuColors.green
        Label {
            anchors.centerIn: parent
            text: base.title
            color: "#ffffff"
        }
        AbstractButton {
            anchors.fill: parent
            onClicked: base.closed()
        }
    }
}
