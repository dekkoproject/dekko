import QtQuick 2.4
import Ubuntu.Components 1.3
import Dekko.Components 1.0

AbstractButton {
    height: units.gu(4)
    width: parent.width

    property alias checked: checkbox.checked
    property alias text: label.text
    property alias textColor: label.color
    StretchRow{
        anchors {
            fill: parent
        }
        spacing: units.gu(1)
        CheckBox {
            id: checkbox
            anchors.verticalCenter: parent.verticalCenter
        }
        Label {
            id: label
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    onClicked: checkbox.checked = !checkbox.checked
}
