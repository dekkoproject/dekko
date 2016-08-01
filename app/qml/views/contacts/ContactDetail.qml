import QtQuick 2.4
import Ubuntu.Components 1.3

AbstractButton {
    height: units.gu(4)
    width: parent.width
    property string type: ""
    property string value: ""
    visible: value
    Label {
        id: typeLabel
        anchors {
            left: parent.left
            leftMargin: units.gu(2)
            top: parent.top
            topMargin: units.gu(1)
        }
        text: type
        color: UbuntuColors.inkstone
        font.weight: Font.DemiBold
    }
    Label {
        anchors {
            top: typeLabel.bottom
            topMargin: units.gu(1)
            left: parent.left
            leftMargin: units.gu(3)
        }
        text: value
    }
}
