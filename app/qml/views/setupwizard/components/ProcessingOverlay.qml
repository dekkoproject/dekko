import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

PopupBase {
    id: base

    property alias text: label.text

    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.jet
        opacity: 0.9
    }

    Item {
        anchors.centerIn: parent
        width: parent.width
        height: label.height + spinner.height + units.gu(3)
        Label {
            id: label
            color: "#FFFFFF"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        ActivityIndicator {
            id: spinner
            running: true
            anchors {
                top: label.bottom
                topMargin: units.gu(3)
                horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
